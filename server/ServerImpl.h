#pragma once
#include <memory>
#include "foxtrot.pb.h"
#include "foxtrot.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <grpc++/security/credentials.h>
#include <string>
#include "DeviceHarness.h"
#include "Logging.h"

#include <future>
#include <exception>
#include <condition_variable>
#include <mutex>

#include <boost/variant.hpp>

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerBuilder;


namespace foxtrot{
    typedef boost::variant<double,int,bool,std::string> ft_variant;
    using flagmap =  std::map<std::string, ft_variant> ;

class ServerImpl 
{
public:
    ServerImpl(const std::string& servcomment, DeviceHarness& harness);
    ServerImpl(const std::string& servcomment, DeviceHarness& harness, const std::string& connstr);
    
    ~ServerImpl();
    void Run();
    void HandleRpcs();
    
    std::vector<std::future<std::exception_ptr>> RunMultithread(int nthreads);
    
    int join_multithreaded();
    
    void SetupSSL(const std::string& crtfile, const std::string& keyfile,
		  bool force_client_auth = false);
    
private:
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
    DeviceHarness& _harness;
    
    Logging _lg;
    
    std::shared_ptr<grpc::ServerCredentials> _creds = nullptr;
    
};


}
