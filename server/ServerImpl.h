#pragma once
#include <memory>
#include "foxtrot.pb.h"
#include "foxtrot.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <string>

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerBuilder;


namespace foxtrot{

class ServerImpl final
{
public:
    ServerImpl(const std::string& servcomment);
    
    ~ServerImpl();
    void Run();
    void HandleRpcs();
    
private:
    std::unique_ptr<ServerCompletionQueue> _cq;
    std::unique_ptr<Server> _server;
    
    foxtrot::exptserve::AsyncService _service;
    
    std::string _servcomment;
    
    
};

}
