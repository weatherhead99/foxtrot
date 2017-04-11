#pragma once
#include "client.h"
#include <memory>
#include <chrono>
#include "Logging.h"
#include <functional>
#include <map>
#include <future>
#include <tuple>

typedef std::function<foxtrot::ft_variant(foxtrot::Client&)> telemfun;

namespace foxtrot
{
    class TelemetryServer
    {
    public:
        TelemetryServer(const std::string& topic, foxtrot::Client& client);
        
        ~TelemetryServer();
        
        void AddTelemetryItem(telemfun fun, std::chrono::milliseconds timeout, const std::string& name, const std::string& subtopic ="");
        
        std::future<std::exception_ptr> runserver();
        
        
    private:
        foxtrot::Client& _client;
        std::exception_ptr runforever();
        void sort_funs_vector();
        
        
        std::string _topic;
        std::vector<std::tuple<std::chrono::milliseconds, telemfun, std::string, std::string>> _funs;
        int _nn_pub_skt;
        foxtrot::Logging _lg;
        
    };
    

}
