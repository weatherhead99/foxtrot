#include "ServerImpl.h"
#include <string>
#include "CallData.h"

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
    builder.RegisterService(&_service);
    
    _cq = builder.AddCompletionQueue();
    _server = builder.BuildAndStart();
    
    std::cout << "server listening on " << addrstr << std::endl;
    
    HandleRpcs();
}

void foxtrot::ServerImpl::HandleRpcs()
{
    new CallData(&_service, _cq.get());
    void* tag;
    bool ok;
    while(true)
    {
        GPR_CODEGEN_ASSERT(_cq->Next(&tag,&ok));
        GPR_CODEGEN_ASSERT(ok);
        static_cast<CallData*>(tag)->Proceed();
        
    }
        
    
}
