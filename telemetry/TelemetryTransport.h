#pragma once

#include "client.h"
#include "Logging.h"
#include <memory>
#include <string>

namespace foxtrot {

struct TelemetryMessage
    {
      std::string name;
      std::string subtopic;
      std::string payload;
      ft_variant val;
      
      TelemetryMessage(const foxtrot::telemetry& telem_msg,
          foxtrot::Logging* lg = nullptr);
      TelemetryMessage();
    };
    
    class TelemetryTransport
    {
    public:
        TelemetryTransport(const std::string& topic);
        virtual ~TelemetryTransport();
        virtual void BroadcastTelemetry(const TelemetryMessage& msg) = 0;
        
        void setTopic(const std::string& topic);
        const std::string& getTopic() const;
        
    protected:
        foxtrot::Logging _lg;
        
    private:
        std::string _topic;
    };
    
}
