#include "TelemetryTransport.h"

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


foxtrot::TelemetryTransport::TelemetryTransport(const std::string& topic)
: _lg("TelemetryTransport"), _topic(topic)
{
}

foxtrot::TelemetryTransport::~TelemetryTransport()
{
}

void foxtrot::TelemetryTransport::setTopic(const std::string& topic)
{
    _topic = topic;
}

const std::string& foxtrot::TelemetryTransport::getTopic() const
{
    return _topic;
}


