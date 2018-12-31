#pragma once 

#include "TelemetryTransport.h"

#include <mosquittopp.h>
#include "Logging.h"


namespace detail {
 
    class mosquitto_adaptor : public mosqpp::mosquittopp
    {
    public:
        mosquitto_adaptor();
    private:
        foxtrot::Logging _lg;
    };
    
}

namespace foxtrot {
    
    class MQTTTransport : public TelemetryTransport
    {
    public:
        MQTTTransport(const std::string& topic);
        ~MQTTTransport() override;
        
        void ConnectToBroker(const std::string& addr, int port);
        
        void BroadcastTelemetry(const TelemetryMessage& msg) override;
      
    private:
        detail::mosquitto_adaptor adaptor_;
    
        
    };
    
    
}
