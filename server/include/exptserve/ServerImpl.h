#pragma once
#include <memory>
#include <string>
#include <future>
#include <exception>
#include <condition_variable>
#include <mutex>

#include <grpc++/grpc++.h>
#include <grpc++/security/credentials.h>

#include <variant>

#include <foxtrot/foxtrot.pb.h>
#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/server/DeviceHarness.h>
#include <foxtrot/server/AuthHandler.h>


#include "HandlerBase.h"
#include "pushbullet_api.hh"

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerBuilder;

using std::string;

namespace foxtrot{
    using ft_std_variant = std::variant<double,int,bool,std::string>;
    using flagmap =  std::map<std::string, ft_std_variant> ;
    
class ServerImpl 
{
public:
    ServerImpl(const std::string& servcomment, std::shared_ptr<DeviceHarness> harness);
    ServerImpl(const std::string& servcomment, std::shared_ptr<DeviceHarness> harness, const std::string& connstr);
    
    void setup_notifications(const string& apikey, const string& default_title, const string& default_channel);
    void setup_auth(const std::string& credsfile, int creds_validity_seconds);
    
    ~ServerImpl();
    void Run();
    void HandleRpcs();
    
    std::vector<std::future<std::exception_ptr>> RunMultithread(int nthreads);
    
    int join_multithreaded();
    
    void SetupSSL(const std::string& crtfile, const std::string& keyfile,
		  bool force_client_auth = false);
    
private:
    template<typename T, typename... initargs> void add_logic(initargs&&... args)
    {
        std::shared_ptr<T> logic(new T(std::forward<initargs>(args)...));
        new HandlerBase<T>(&_service, _cq.get(), logic);
    };
    
    bool notifications_enabled = false;
    bool auth_enabled = false;
    std::shared_ptr<flagmap> _serverflags;
    
    std::string _connstr;
  
    void setup_common(const std::string& addrstr );
  
    std::mutex _exitmut;
    std::condition_variable _condvar;
    
    int _exitthreadnum;
    
    std::unique_ptr<ServerCompletionQueue> _cq;
    std::unique_ptr<Server> _server;
    
    foxtrot::exptserve::AsyncService _service;
    
    std::string _servcomment;
    std::shared_ptr<DeviceHarness> _harness;
    
    std::string default_channel_;
    std::string default_title_;
    
    Logging _lg;
    std::shared_ptr<AuthHandler> _auth_api = nullptr;
    std::unique_ptr<pushbullet_api> _noti_api = nullptr;
    std::shared_ptr<grpc::ServerCredentials> _creds = nullptr;
    
};


}
