#include "ServerImpl.h"
#include <string>

#include "ServerDescribeImpl.h"
#include "InvokeCapabilityImpl.h"
#include "FetchDataImpl.h"


#include <iostream>
#include <typeinfo>
#include <future>

using std::string;
using namespace foxtrot;

foxtrot::ServerImpl::ServerImpl(const std::string& servcomment, foxtrot::DeviceHarness& harness)
: _servcomment(servcomment), _harness(harness), _lg("ServerImpl")
{
}



ServerImpl::~ServerImpl()
{
    _server->Shutdown();
    _cq->Shutdown();
    
}

void ServerImpl::setup_common(const std::string& addrstr)
{
    
    
    ServerBuilder builder;
    //TODO: SECURE CREDENTIALS!
    builder.AddListeningPort(addrstr,grpc::InsecureServerCredentials());
    
    //TODO: Register Service
    builder.RegisterService(&_service);
    
    _cq = builder.AddCompletionQueue();
    _server = builder.BuildAndStart();
    std::cout << "server listening on " << addrstr << std::endl;
    

}



void ServerImpl::Run()
{
    setup_common("0.0.0.0:50051");
    
    HandleRpcs();
}

std::vector< std::future< std::__exception_ptr::exception_ptr > > ServerImpl::RunMultithread(int nthreads)
{
  setup_common("0.0.0.0:50051");
  
  auto handlerpccatch = [this] () {  
    try
    {
      HandleRpcs();
    }
    catch(...)
    {
      return std::current_exception();
    }
    
    return static_cast<std::exception_ptr>(nullptr);
    
  };
  
  std::vector<std::future<std::exception_ptr>> out;
  out.reserve(nthreads);
  for(auto i =0; i < nthreads; i++)
  {
    out.push_back(std::async(std::launch::async,handlerpccatch)); 
  }
  
  return out;

}


void foxtrot::ServerImpl::HandleRpcs()
{
    ServerDescribeLogic describe_logic(_servcomment,_harness);
    InvokeCapabilityLogic capability_logic(_harness);
    FetchDataLogic fetch_logic(_harness);
    
    new ServerDescribeImpl(&_service,_cq.get(),describe_logic);
    new InvokeCapabilityImpl(&_service,_cq.get(),capability_logic);
    new FetchDataImpl(&_service,_cq.get(),fetch_logic);
    
    void* tag;
    bool ok;
    
    while(true)
    {
      //TODO: check this for return, shutdown etc
     if(_cq->Next(&tag,&ok))
     {
         if(!ok)
         {
             std::cout << "WARNING: NOT OK!" << std::endl;
         }
        static_cast<HandlerTag*>(tag)->Proceed();
     }
     else
     {
         std::cout << ("queue shut down..") << std::endl;
         return;
     }
     
        
    }
    
        
    
}
