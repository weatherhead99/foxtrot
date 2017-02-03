#include "ServerImpl.h"
#include <string>



using std::string;


ServerImpl::~ServerImpl()
{
    _server->Shutdown();
    _cq->Shutdown();
    
}


void ServerImpl::Run()
{
    //TODO: SETTING OF ADDRESS PROPERLY
    string addrstr("0.0.0.0:50051");
    
    ServerBuilder builder;
    //TODO: SECURE CREDENTIALS!
    builder.AddListeningPort(addrstr,grpc::InsecureServerCredentials());
    
    //TODO: Register Service
    
    _cq = builder.AddCompletionQueue();
    _server = builder.BuildAndStart();
    
    
}
