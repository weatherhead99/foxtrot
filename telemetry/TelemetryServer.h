#pragma once
#include "client.h"
#include <memory>
#include <chrono>
#include "Logging.h"

namespace foxtrot
{
    class TelemetryServer
    {
    public:
        TelemetryServer(foxtrot::Client& client, std::chrono::milliseconds& default_duration, const std::string& servname);
        
        ~TelemetryServer();
        
        
        
    private:
        foxtrot::Client& _client;
        std::string _topic;
        
        int _nn_pub_skt;
        foxtrot::Logging _lg;
        
    };
    

}
