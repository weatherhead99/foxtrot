#include "TelemetryServer.h"
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <algorithm>
#include <thread>
#include <boost/date_time.hpp>
#include <boost/variant.hpp>
#include "foxtrot.pb.h"

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


foxtrot::TelemetryServer::TelemetryServer( const std::string& topic, foxtrot::Client& client)
:  _topic(topic), _lg("TelemetryServer"), _client(client)
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


void foxtrot::TelemetryServer::AddTelemetryItem(telemfun fun, std::chrono::milliseconds timeout, const std::string& name, const std::string& subtopic)
{
    _funs.push_back(std::make_tuple(timeout,fun,name,subtopic));
    
}


std::future<std::exception_ptr> foxtrot::TelemetryServer::runserver()
{
    return std::async(std::launch::async, [this]() { return runforever();} );
    
}

std::exception_ptr foxtrot::TelemetryServer::runforever()
{
    sort_funs_vector();
    
    auto epoch = boost::posix_time::from_time_t(0);
    
    try{
        while(true)
        {
            
        for(auto funtup = _funs.begin(); funtup != _funs.end(); funtup++)
        {
          _lg.Debug("running function for telemetry item: " + std::get<2>(*funtup));  
          auto now = boost::posix_time::microsec_clock::universal_time();
          auto ms_since_epoch = (now - epoch).total_milliseconds();
          _lg.Debug("timestamp: " + std::to_string(ms_since_epoch));
          
          auto ret = std::get<1>(*funtup)(_client);
          
          foxtrot::telemetry telem_msg;
          telem_msg.set_name(std::get<2>(*funtup));
          telem_msg.set_tstamp(ms_since_epoch);
          
          telem_visitor v(telem_msg);
          boost::apply_visitor(v,ret);
          
          std::ostringstream oss;
          oss << _topic << "|" << std::get<3>(*funtup) << "|";
          
          if( !telem_msg.SerializeToOstream(&oss))
          {
              throw std::runtime_error("failed to serialize telemetry message!");
          }
          
          auto nbytes = nn_send(_nn_pub_skt,oss.str().c_str(), oss.str().size(),0);
          if(nbytes < oss.str().size())
          {
              throw std::runtime_error("didn't successfully send all data!");
          };
          
          std::chrono::milliseconds nextwait;
          if(funtup != (_funs.end() -1) )
          {
            nextwait = std::get<0>(*funtup) - std::get<0>(*(funtup+1));
          }
          else
          {
           nextwait = std::get<0>(_funs[0]);   
          }
              
          _lg.Trace("next wait is: " + std::to_string(nextwait.count()));
          std::this_thread::sleep_for(nextwait);
            
        };
        
        
        }
    }
    catch(...)
    {
        
     return std::current_exception();   
        
    }
    
    
    
}


void foxtrot::TelemetryServer::sort_funs_vector()
{
    using funpair = std::tuple<std::chrono::milliseconds, telemfun, std::string, std::string>;
    
    auto cmpfun = [] (const funpair& first, const funpair& second)
    {
      return std::get<0>(first) < std::get<0>(second);
    };
    
    std::sort(_funs.begin(), _funs.end(), cmpfun);
    
}

