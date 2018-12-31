#pragma once
#include "TelemetryTransport.h"

namespace foxtrot {
 
        
    class NanomsgTransport : public TelemetryTransport
    {
    public:
        NanomsgTransport(const std::string& topic);
        ~NanomsgTransport() override;
        void BindSocket(const std::string& bindaddr);
        void BroadcastTelemetry(const TelemetryMessage& msg) override;
        
        void setTopic(const std::string& topic);
        const std::string& getTopic() const;
        
    private:
        int _bindid = 0;
        int _nn_pub_skt = 0;
        std::string _topic;
        
    };
    
    
}
