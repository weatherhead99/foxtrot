#pragma once
#include <memory>
#include "foxtrot.pb.h"
#include "foxtrot.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <string>
#include "DeviceHarness.h"
#include "Logging.h"

#include <future>
#include <exception>
#include <condition_variable>
#include <mutex>

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerBuilder;


namespace foxtrot{

class ServerImpl final
{
public:
    ServerImpl(const std::string& servcomment, DeviceHarness& harness);
    
    ~ServerImpl();
    void Run();
    void HandleRpcs();
    
    std::vector<std::future<std::exception_ptr>> RunMultithread(int nthreads);
    
    int join_multithreaded();
    
private:
  
  
  
    void setup_common(const std::string& addrstr);
  
    std::mutex _exitmut;
    std::condition_variable _condvar;
    
    int _exitthreadnum;
    
    std::unique_ptr<ServerCompletionQueue> _cq;
    std::unique_ptr<Server> _server;
    
    foxtrot::exptserve::AsyncService _service;
    
    std::string _servcomment;
    DeviceHarness& _harness;
    
    Logging _lg;
};

}
