#include "ServerImpl.h"
#include <string>

#include "ServerDescribeImpl.h"
#include "InvokeCapabilityImpl.h"

#include <iostream>
#include <typeinfo>

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
    ServerDescribeLogic describe_logic;
    InvokeCapabilityLogic capability_logic;
    
    new ServerDescribeImpl(&_service,_cq.get(),describe_logic);
    new InvokeCapabilityImpl(&_service,_cq.get(),capability_logic);
    
    void* tag;
    bool ok;
    
    while(true)
    {
     _cq->Next(&tag,&ok);
     static_cast<HandlerTag*>(tag)->Proceed();
        
    }
    
        
    
}
