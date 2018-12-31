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

foxtrot::TelemetryMessage::TelemetryMessage()
{}

foxtrot::TelemetryMessage::TelemetryMessage(const foxtrot::telemetry& telem_msg, foxtrot::Logging* lg)
{
    name = telem_msg.name();
    
    if(!telem_msg.SerializeToString(&payload))
    {
        if(lg)
        {
            lg->Error("failed to serialize message!");
            throw std::runtime_error("failed to serialize protobuf message!");
        }
        
    };
    
}



foxtrot::TelemetryTransport::TelemetryTransport()
: _lg("TelemetryTransport")
{
}

foxtrot::TelemetryTransport::~TelemetryTransport()
{
}


template<typename funtp>void nn_err_proc(funtp&& fun, foxtrot::Logging& lg)
{
  auto ret = fun();
  if(ret == -1)
  {
      auto sterr = std::string(strerror(errno));
      lg.strm(sl::error) << "nanomsg error: " << sterr;
      throw std::runtime_error("nanomsg error: " + sterr);
  };
};


foxtrot::NanomsgTransport::NanomsgTransport(const std::string& topic):
_topic(topic)
{
    
    nn_err_proc( [this]() {        this->_nn_pub_skt = nn_socket(AF_SP,NN_PUB);
                                    return this->_nn_pub_skt;} , _lg);

}

foxtrot::NanomsgTransport::~NanomsgTransport()
{
    
    if(_bindid > 0)
    {
        nn_err_proc( [this]() 
        { return nn_shutdown(this->_nn_pub_skt, _bindid);}, _lg);
    }
       
    if(_nn_pub_skt > 0)
    {
        nn_err_proc( [this] ()
        { return nn_close(this->_nn_pub_skt); } , _lg);
        
    };
        
    
}

void foxtrot::NanomsgTransport::BindSocket(const std::string& bindaddr)
{
    nn_err_proc( [this, &bindaddr] () 
        { return nn_bind(this->_nn_pub_skt, bindaddr.c_str()); }, _lg);
    
    _lg.strm(sl::debug) << "endpoint id: " + std::to_string(_bindid);
    
}


void foxtrot::NanomsgTransport::BroadcastTelemetry(const foxtrot::TelemetryMessage& msg)
{
    std::ostringstream oss;
    //TODO: this is where you implement the binary/ASCII transport
    oss << _topic << "|" << msg.subtopic << "|" << msg.name << ">" << msg.payload;
    
    _lg.Trace("topic string: " + oss.str());
    
    auto nbytes = nn_send(_nn_pub_skt, oss.str().c_str(), oss.str().size(),0);
    
    if(nbytes != oss.str().size())
    {
        _lg.Error("invalid number of bytes written!");
        _lg.strm(sl::error) << "expected: " << oss.str().size() << ", actual: " << nbytes;
    }
    
};

void foxtrot::NanomsgTransport::setTopic(const std::string& topic)
{
    _topic = topic;
}

const std::string& foxtrot::NanomsgTransport::getTopic() const
{
    return _topic;
}


foxtrot::TelemetryServer::TelemetryServer( const std::string& topic, foxtrot::Client& client, int tick_ms)
:  _lg("TelemetryServer"), _client(client), _tick_ms(tick_ms)
{
    _legacy = true;
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    _transport.reset(new NanomsgTransport(topic));
    
    
}

foxtrot::TelemetryServer::TelemetryServer(foxtrot::Client& client, 
                                          std::unique_ptr<TelemetryTransport> transport,
                                          int tick_ms
                                         )
: _lg("TelemetryServer") , _client(client), _tick_ms(tick_ms)
{
    _transport = std::move(transport);
};

foxtrot::TelemetryServer::~TelemetryServer()
{
    
}


void foxtrot::TelemetryServer::AddTelemetryItem(telemfun fun, unsigned ticks, const std::string& name, const std::string& subtopic)
{
    _funs.push_back(std::make_tuple(ticks,fun,name,subtopic,true));
    
}


void foxtrot::TelemetryServer::AddNonTelemetryItem(telemfun fun, unsigned int ticks, const std::string& name)
{
    _funs.push_back(std::make_tuple(ticks,fun,name,"",false));
}


void foxtrot::TelemetryServer::BindSocket(const std::string& bindaddr)
{
    if(_legacy)
        static_cast<NanomsgTransport*>(_transport.get())->BindSocket(bindaddr);
    else
    {
        _lg.Error("non legacy transport not implemented yet!");
        throw std::logic_error("non legacy transport not implemented yet!");
    }
    
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
                    _lg.Trace("which return case: " + std::to_string(msg.return_case()));
                    success = true;
                  }
                  catch(...)
                    {
                        foxtrot_rpc_error_handling(std::current_exception(),msg,_lg,nullptr); 
                    }
                  
                  auto now = boost::posix_time::microsec_clock::universal_time();
                  msg.set_name(std::get<2>(funtup));
                  msg.set_tstamp( (now-epoch).total_microseconds());
                  
                  //note could throw...
                  TelemetryMessage send_msg;
                  
                  try{
                      send_msg = TelemetryMessage{msg};
                  }
                  catch(std::runtime_error& err)
                  {
                      _lg.Debug("couldn't serialize, sending anyway...");
                  };
                  
                  send_msg.subtopic = std::get<3>(funtup);
                  
                    if(std::get<4>(funtup))
                    {
                        _lg.Trace("transmitting telemetry message...");
                        _transport->BroadcastTelemetry(send_msg);
                        
                    }
                    else
                    {
                        _lg.Trace("non-transmission.");
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
    
    using funpair = decltype(_funs)::value_type;
    
    
    auto cmpfun = [] (const funpair& first, const funpair& second)
    {
      return std::get<0>(first) < std::get<0>(second);
    };
    
    std::sort(_funs.begin(), _funs.end(), cmpfun);
    
}

void foxtrot::TelemetryServer::set_tick_ms(int tick_ms)
{
  _tick_ms = tick_ms;

}

void foxtrot::TelemetryServer::set_topic(const std::string& topic)
{
  if(_legacy)
  {
      static_cast<NanomsgTransport*>(_transport.get())->setTopic(topic);
  }
  else
  {
      _lg.Error("non legacy transport not implemented yet!");
      throw std::logic_error("non legacy transport not implemented yet!");
  }
}

