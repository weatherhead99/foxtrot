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
: _servcomment(servcomment), _harness(harness), _lg("ServerImpl"), _connstr("0.0.0.0:50051")
{
}

ServerImpl::ServerImpl(const string& servcomment, DeviceHarness& harness, const string& connstr)
: ServerImpl(servcomment,harness)
{
  _connstr = connstr;

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
    _lg.Info("server listening on " + addrstr );
    
    std::shared_ptr<ServerDescribeLogic> describe_logic(new ServerDescribeLogic(_servcomment,_harness));
    std::shared_ptr<InvokeCapabilityLogic> capability_logic(new InvokeCapabilityLogic(_harness));	
    std::shared_ptr<FetchDataLogic> fetch_logic(new FetchDataLogic(_harness));
    
    new ServerDescribeImpl(&_service,_cq.get(),describe_logic);
    new InvokeCapabilityImpl(&_service,_cq.get(),capability_logic);
    new FetchDataImpl(&_service,_cq.get(),fetch_logic);
}



void ServerImpl::Run()
{
    setup_common(_connstr);
        
    HandleRpcs();
}

std::vector< std::future< std::__exception_ptr::exception_ptr > > ServerImpl::RunMultithread(int nthreads)
{
  setup_common(_connstr);
  
  std::vector<std::future<std::exception_ptr>> out;
  out.reserve(nthreads);
  for(auto i =0; i < nthreads; i++)
  {
    auto handlerpccatch = [this,i] () {  
      std::exception_ptr except;
      try
      {  
	HandleRpcs();
      }
      catch(...)
      {
	_lg.Error("caught exception in handlerpclambda");
	except = std::current_exception(); 
      }
      
      _exitthreadnum = i;
      _condvar.notify_all();
      
      return except;
      
    };
        
    _lg.Debug("starting server thread...");
    out.push_back(std::async(std::launch::async,handlerpccatch)); 
  }
  
  return out;

}

int ServerImpl::join_multithreaded()
{
  std::unique_lock<std::mutex> lock(_exitmut);
  _lg.Debug("waiting on exit condition...");
  _condvar.wait(lock);
  _lg.Info("thread " + std::to_string(_exitthreadnum) + " exited");
  
  return _exitthreadnum;
}




void foxtrot::ServerImpl::HandleRpcs()
{
    
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
