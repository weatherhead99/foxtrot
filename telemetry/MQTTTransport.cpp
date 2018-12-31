#include "MQTTTransport.h"
#include "TelemetryError.h"

//TODO: callback stuffs

void mqtt_error_handling(int ret, foxtrot::Logging& lg)
{
    if(ret)
    {
        std::ostringstream oss;
        oss << "mosquitto library error: " << mosqpp::strerror(ret);
        lg.Error(oss.str());
        throw foxtrot::TelemetryError(oss.str());
    }
    
};


detail::mosquitto_adaptor::mosquitto_adaptor(const char* id, bool clean_session)
: _lg("mosquitto_adaptor"), mosquittopp(id,clean_session)
{
    mosqpp::lib_init();
    loop_start();
    
}

detail::mosquitto_adaptor::~mosquitto_adaptor()
{
    loop_stop(false);
    mosqpp::lib_cleanup();
}


foxtrot::MQTTTransport::MQTTTransport(const std::string& topic)
: foxtrot::TelemetryTransport(topic)
{
    int maj, min, pat;
    mosqpp::lib_version(&maj,&min,&pat);
    _lg.strm(sl::info) << "mosquitto library version: " << maj << "." << min << "." << pat; 
}

foxtrot::MQTTTransport::~MQTTTransport()
{
    adaptor_.disconnect();
}

void foxtrot::MQTTTransport::ConnectToBroker(const std::string& addr, int port, int keepalive)
{
    auto ret = adaptor_.connect(addr.c_str(), port, keepalive);
    mqtt_error_handling(ret, _lg);
        
}

void foxtrot::MQTTTransport::BroadcastTelemetry(const foxtrot::TelemetryMessage& msg)
{
    std::ostringstream oss;
    oss << msg.subtopic << "|" << msg.name << ">" << msg.payload;
    
    _lg.strm(sl::trace) << "message string: " << oss.str();
    

    void* paylptr = const_cast<void*>(reinterpret_cast<const void*>(oss.str().data()));
    
    //TODO: something with message id?
    auto ret = adaptor_.publish(nullptr, getTopic().c_str(), oss.str().size(), paylptr, 0);
    mqtt_error_handling(ret,_lg);
    
}
