#pragma once

#include <memory>
#include <string>
#include <future>
#include <exception>
#include <condition_variable>
#include <mutex>
#include <map>
#include <typeinfo>

#include <grpc++/grpc++.h>
#include <grpc++/security/credentials.h>


#include <foxtrot/foxtrot.pb.h>
#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/ft_sessions.grpc.pb.h>
#include <foxtrot/ft_capability.grpc.pb.h>
#include <foxtrot/ft_auth.grpc.pb.h>
#include <foxtrot/ft_flags.grpc.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/DeviceHarness.h>
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/server/FlagMap.hh>
#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/server/auth_layer/AuthBase.hh>

#include "HandlerBase.h"
#include "pushbullet_api.hh"

#include "ServerImpl.h"

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerBuilder;

using std::string;

namespace foxtrot{

    class ServerImplLegacy : public ServerImpl
{
public:
    ServerImplLegacy(const std::string& servcomment, std::shared_ptr<DeviceHarness> harness);
    ServerImplLegacy(const std::string& servcomment, std::shared_ptr<DeviceHarness> harness, const std::string& connstr);
    
    void setup_notifications(const string& apikey, const string& default_title, const string& default_channel);
    void setup_auth(const std::string& credsfile, int creds_validity_seconds);
    
    ~ServerImplLegacy();
    void Run();
    void HandleRpcs();
    
    std::vector<std::future<std::exception_ptr>> RunMultithread(int nthreads);
    
    int join_multithreaded();
    
    void SetupSSL(const std::string& crtfile, const std::string& keyfile,
		  bool force_client_auth = false);
    
    template<typename T> T* register_get_service()
    {
        std::size_t tphash = typeid(T).hash_code();
//         _lg.strm(sl::trace) << "tphash: "  << tphash << " name: " << typeid(T).name();
        auto mit = _services.find(tphash);
        if(mit == _services.end())
        {
            _lg.Debug("registering new service...");
            auto newserv = new T();
            _services[tphash] = reinterpret_cast<void*>(newserv);
            builder.RegisterService(newserv);
            return newserv;
        }
        
        return reinterpret_cast<T*>(mit->second);
    };
    
    
private:
    
    ServerCompletionQueue* getCQ();
    
    struct logic_add_helper_base{
        virtual ~logic_add_helper_base() {};
    };
    
    template<typename T> 
    struct logic_add_helper : public logic_add_helper_base
    {
      using Service = typename T::servicetp;
      template<typename... initargs>
      logic_add_helper(ServerImplLegacy& impl, initargs&&... args)
      : _impl(impl), _lg("logic_add_helper")
      {
          _lg.Trace("creating new logic object");
          _logic = std::make_shared<T>( std::forward<initargs>(args)...);
          _impl.add_logic_shared_ptr(_logic);
          _impl.register_get_service<Service>();
      }
      
      virtual ~logic_add_helper()
      {
          _lg.Trace("creating new HandlerBase");
          new HandlerBase<T>(_impl.register_get_service<Service>(), _impl.getCQ(), _logic);
      }
      
    private:
        foxtrot::Logging _lg;
        ServerImplLegacy& _impl;
        std::shared_ptr<T> _logic;
         
    };
    
    template<typename T, typename... initargs>
    std::unique_ptr<logic_add_helper<T>> create_logic_add_helper(initargs&&... args)
    {
        return std::make_unique<logic_add_helper<T>>(*this, std::forward<initargs>(args)...);
    };
    
    
    template<typename T>
    void add_logic_shared_ptr(std::shared_ptr<T> logic)
    {
        _logics.push_back(std::static_pointer_cast<void>(logic));
        
    }
    
    template<typename T, typename... initargs> void add_logic(initargs&&... args)
    {
        using Service = typename T::servicetp;
        auto serv = register_get_service<Service>();
        std::shared_ptr<T> logic(new T(std::forward<initargs>(args)...));
        new HandlerBase<T>(serv, _cq.get(), logic);
    };
    
    template<typename T, typename Service, typename... initargs> void add_logic_with_service(Service* serv, initargs&&... args)
    {
        std::shared_ptr<T> logic(new T(std::forward<initargs>(args)...));
        new HandlerBase<T,Service>(serv, _cq.get(), logic);
    }
    
    bool notifications_enabled = false;
    bool auth_enabled = false;
    std::shared_ptr<FlagMap> _serverflags;
    
    std::string _connstr;
  
    void setup_common(const std::string& addrstr );
  
    std::mutex _exitmut;
    std::condition_variable _condvar;
    
    int _exitthreadnum;
    
    std::unique_ptr<ServerCompletionQueue> _cq;
    std::unique_ptr<Server> _server;
    
    std::map<std::size_t, void*> _services;
    
    foxtrot::exptserve::AsyncService _service;
    foxtrot::sessions::AsyncService _sessionservice;
    foxtrot::capability::AsyncService _capabilityservice;
    foxtrot::flags::AsyncService _flagservice;
    foxtrot::auth::AsyncService _authservice;
    
    std::shared_ptr<SessionManager> _sesman;
    
    grpc::ServerBuilder builder;
    std::vector<std::shared_ptr<void>> _logics;
    
    std::string _servcomment;
    std::shared_ptr<DeviceHarness> _harness;
    
    std::string default_channel_;
    std::string default_title_;
    
    Logging _lg;
    std::shared_ptr<AuthHandler> _auth_api = nullptr;
    std::unique_ptr<pushbullet_api> _noti_api = nullptr;
    std::shared_ptr<grpc::ServerCredentials> _creds = nullptr;
    std::shared_ptr<UserAuthInterface> _auth_iface = nullptr;
    
};


}

