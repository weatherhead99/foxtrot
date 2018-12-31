#include "NanomsgTransport.h"
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

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
    else
    {
        _lg.strm(sl::trace) << "wrote: " << nbytes << "bytes";
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


