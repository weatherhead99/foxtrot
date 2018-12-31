#pragma once
#include "client.h"
#include <memory>
#include <chrono>
#include "Logging.h"
#include <functional>
#include <map>
#include <future>
#include <tuple>
#include "TelemetryTransport.h"

typedef std::function<foxtrot::ft_variant(foxtrot::Client&)> telemfun;


namespace foxtrot
{
    
    
    class TelemetryServer
    {
    public:
        TelemetryServer(foxtrot::Client& client, std::unique_ptr<TelemetryTransport> transport, int tick_ms);
        
        
        ~TelemetryServer();
        
        void AddTelemetryItem(telemfun fun, unsigned ticks, const std::string& name, const std::string& subtopic ="");
        void AddNonTelemetryItem(telemfun fun, unsigned ticks, const std::string& name);
	
        std::future<std::exception_ptr> runserver();
        
	
        void set_tick_ms(int tick_ms);
    
    private:
        foxtrot::Client& _client;
        std::exception_ptr runforever();
        void sort_funs_vector();
        
        std::unique_ptr<TelemetryTransport> _transport;
        
        int _tick_ms;
        std::vector<std::tuple<unsigned, telemfun, std::string, std::string,bool>> _funs;
        foxtrot::Logging _lg;
        
        bool _legacy = false;
        
    };
    

}
