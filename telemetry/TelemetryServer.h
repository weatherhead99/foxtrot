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
      
      TelemetryMessage(const foxtrot::telemetry& telem_msg,
          foxtrot::Logging* lg = nullptr);
      TelemetryMessage();
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
        
        void setTopic(const std::string& topic);
        const std::string& getTopic() const;
        
    private:
        int _bindid = 0;
        int _nn_pub_skt = 0;
        std::string _topic;
        
    };
    
    
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
