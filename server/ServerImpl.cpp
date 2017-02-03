#include "ServerImpl.h"
#include <string>

#include <iostream>

using std::string;
using namespace foxtrot;

ServerImpl::~ServerImpl()
{
    _server->Shutdown();
    _cq->Shutdown();
    
}


void ServerImpl::Run()
{
    //TODO: SETTING OF ADDRESS PROPERLY
    string addrstr("0.0.0.0:50051");
    
    exptserve::AsyncService service;
    
    ServerBuilder builder;
    //TODO: SECURE CREDENTIALS!
    builder.AddListeningPort(addrstr,grpc::InsecureServerCredentials());
    
    //TODO: Register Service
    
    _cq = builder.AddCompletionQueue();
    _server = builder.BuildAndStart();
    
    std::cout << "server listening on " << addrstr << std::endl;
    
    
}
