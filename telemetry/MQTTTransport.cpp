#include "MQTTTransport.h"


detail::mosquitto_adaptor::mosquitto_adaptor()
: _lg("mosquitto_adaptor")
{
    
}

foxtrot::MQTTTransport::MQTTTransport(const std::string& topic)
: foxtrot::TelemetryTransport(topic)
{
    
}

foxtrot::MQTTTransport::~MQTTTransport()
{
}

void foxtrot::MQTTTransport::ConnectToBroker(const std::string& addr, int port)
{
}

void foxtrot::MQTTTransport::BroadcastTelemetry(const foxtrot::TelemetryMessage& msg)
{
    std::ostringstream oss;
    oss << msg.subtopic << "|" << msg.name << ">" << msg.payload;
    
    _lg.strm(sl::trace) << "message string: " << oss.str();
    

    void* paylptr = const_cast<void*>(reinterpret_cast<const void*>(oss.str().data()));
    
    //TODO: something with message id?
    auto ret = adaptor_.publish(nullptr, getTopic().c_str(), oss.str().size(), paylptr, 0);
    
    if(ret)
    {
        
        
    };
    
}
