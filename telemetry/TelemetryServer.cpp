#include "TelemetryServer.h"
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>


foxtrot::TelemetryServer::TelemetryServer(foxtrot::Client& client, std::chrono::milliseconds& default_duration, const std::string& topic)
: _client(client), _topic(topic), _lg("TelemetryServer")
{
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
