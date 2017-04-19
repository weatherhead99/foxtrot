#include "TelemetryServer.h"
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <algorithm>
#include <thread>
#include <boost/date_time.hpp>
#include <boost/variant.hpp>
#include "foxtrot.pb.h"
#include "ServerUtil.h"
#include "DeviceError.h"
#include "ProtocolError.h"

class telem_visitor : public boost::static_visitor<>
{
public:
    telem_visitor(foxtrot::telemetry& telem_msg): _telem_msg(telem_msg) {};
    
    void operator()(double& d) const
    {
        _telem_msg.set_dblret(d);
        
    };
    void operator()(int& i) const
    {
        _telem_msg.set_intret(i);
        
    };
    void operator()(bool& b) const
    {
        _telem_msg.set_boolret(b);
    };
    void operator()(const std::string& s) const
    {
        _telem_msg.set_stringret(s);
    };
    
private:
    foxtrot::telemetry& _telem_msg;
    
};


foxtrot::TelemetryServer::TelemetryServer( const std::string& topic, foxtrot::Client& client, int tick_ms)
:  _topic(topic), _lg("TelemetryServer"), _client(client), _tick_ms(tick_ms)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    _nn_pub_skt = nn_socket(AF_SP, NN_PUB);
    if(_nn_pub_skt <0)
    {
        _lg.Error("error opening nanomsg socket");
        throw std::runtime_error("nanomsg error! errno: " + std::to_string(errno));
        
    }
    
}

foxtrot::TelemetryServer::~TelemetryServer()
{
    if(_nn_pub_skt >0)
    {
        
        auto ret = nn_close(_nn_pub_skt);   
        if(ret <0)
        {
            _lg.Error("error closing nanomsg socket");
            _lg.Error("code : " + std::to_string(ret));
        }
    }
    
    
}


void foxtrot::TelemetryServer::AddTelemetryItem(telemfun fun, unsigned ticks, const std::string& name, const std::string& subtopic)
{
    _funs.push_back(std::make_tuple(ticks,fun,name,subtopic));
    
}

void foxtrot::TelemetryServer::BindSocket(const std::string& bindaddr)
{
    auto ret = nn_bind(_nn_pub_skt,bindaddr.c_str());
    if(ret < 0)
    {
        throw std::runtime_error("nanomsg error: " + std::string(strerror(errno)));
    }
    
    _lg.Debug("endpoint id: " + std::to_string(ret));
    
}


std::future<std::exception_ptr> foxtrot::TelemetryServer::runserver()
{
    return std::async(std::launch::async, [this]() { return runforever();} );
    
}

std::exception_ptr foxtrot::TelemetryServer::runforever()
{
    sort_funs_vector();
    
    auto epoch = boost::posix_time::from_time_t(0);
    
    long unsigned _tick = 1;
    
    try{
        while(true)
        {
            for(auto& funtup : _funs)
            {
              if(_tick % std::get<0>(funtup) == 0  )
              {
                  _lg.Trace("collecting telemetry: " + std::get<2>(funtup));
                  //this function should be called on this tick
                  auto fun  = std::get<1>(funtup);
                  
                  foxtrot::telemetry msg;
                  foxtrot::ft_variant telem_value;
                  bool success = false;
                  try{
                    telem_value = std::get<1>(funtup)(_client);
                    
                    telem_visitor vst(msg);
                    //TODO error handling here
                    boost::apply_visitor(vst,telem_value);
                    
                    success = true;
                  }
                  catch(class foxtrot::DeviceError& err)
                  {
                      _lg.Error("device error while getting telemetry");
                      set_repl_err(msg,err,error_types::DeviceError);
                      
                  }
                  catch(class foxtrot::ProtocolError& err)
                  {
                      _lg.Error("protocol error while getting telemetry");
                      set_repl_err(msg,err,error_types::ProtocolError);
                      
                  }
                  catch(std::out_of_range& err)
                  {
                      _lg.Error("out of range error trapped while getting telemetry");
                      set_repl_err(msg,err,error_types::out_of_range);
                  }
                  catch(std::exception& err)
                  {
                      _lg.Error("generic exception while getting telemetry");
                      set_repl_err(msg,err,error_types::Error);
                  }
                  catch(...)
                  {
                      _lg.Error("unhandled error while getting telemetry");
                      set_repl_err_msg(msg,"unknown error",error_types::unknown_error);
                  }
                  
                  
                  auto now = boost::posix_time::microsec_clock::universal_time();
                  msg.set_name(std::get<2>(funtup));
                  msg.set_tstamp( (now-epoch).total_microseconds());
                  
                  std::ostringstream oss;
                  
                  oss << _topic << "|" << std::get<3>(funtup) << std::get<2>(funtup);
                  
                  
                  
                  _lg.Trace("topic string: " + oss.str());
                  
                  if(!msg.SerializeToOstream(&oss))
                  {
                      _lg.Error("failed to serialize message!");
                  };
                  
                  auto total_msg = oss.str();
                  
                  auto nbytes = nn_send(_nn_pub_skt, total_msg.c_str(),total_msg.size(),0);
                  
                  if(nbytes != total_msg.size())
                  {
                   _lg.Error("invalid number of bytes written!");   
                   _lg.Error("expected: " + std::to_string(total_msg.size()));
                   _lg.Error("actual: " + std::to_string(nbytes));
                  }
                  
              }
              
            }
            
         _tick++;
         std::this_thread::sleep_for(std::chrono::milliseconds(_tick_ms));
         _lg.Trace("sleeping until next tick..");
        
        }
    }
    catch(...)
    {
        
     return std::current_exception();   
        
    }
    
    
    
}


void foxtrot::TelemetryServer::sort_funs_vector()
{
    using funpair = std::tuple<unsigned, telemfun, std::string, std::string>;
    
    auto cmpfun = [] (const funpair& first, const funpair& second)
    {
      return std::get<0>(first) < std::get<0>(second);
    };
    
    std::sort(_funs.begin(), _funs.end(), cmpfun);
    
}

