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
    
    struct TelemetryMessage
    {
      std::string name;
      std::string subtopic;
      std::string payload;
      ft_variant val;
    };
    
    class TelemetryTransport
    {
    public:
        TelemetryTransport();
        virtual ~TelemetryTransport();
        virtual void BroadcastTelemetry(const TelemetryMessage& msg) = 0;
    protected:
        foxtrot::Logging _lg;
    };
    
    class NanomsgTransport : public TelemetryTransport
    {
    public:
        NanomsgTransport(const std::string& topic);
        ~NanomsgTransport() override;
        void BindSocket(const std::string& bindaddr);
        void BroadcastTelemetry(const TelemetryMessage& msg) override;
        
    private:
        int _bindid = 0;
        int _nn_pub_skt = 0;
        std::string _topic;
        
    };
    
    
    class TelemetryServer
    {
    public:
        TelemetryServer(const std::string& topic, foxtrot::Client& client, int tick_ms);
        TelemetryServer(foxtrot::Client& client, std::shared_ptr<TelemetryTransport> transport);
        
        
        
        ~TelemetryServer();
        
        void BindSocket(const std::string& bindaddr);
        void AddTelemetryItem(telemfun fun, unsigned ticks, const std::string& name, const std::string& subtopic ="");
        void AddNonTelemetryItem(telemfun fun, unsigned ticks, const std::string& name);
	
        std::future<std::exception_ptr> runserver();
        
	
	void set_tick_ms(int tick_ms);
	void set_topic(const std::string& topic);
    
    private:
        foxtrot::Client& _client;
        std::exception_ptr runforever();
        void sort_funs_vector();
        
        int _tick_ms;
        std::string _topic;
        std::vector<std::tuple<unsigned, telemfun, std::string, std::string,bool>> _funs;
        int _nn_pub_skt;
        foxtrot::Logging _lg;
        
    };
    

}
