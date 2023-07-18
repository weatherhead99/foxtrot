#include <string>
#include <fstream>
//might be needed on windows?
#include <exception>
#include <iostream>
#include <typeinfo>

#include <foxtrot/server/AuthHandler.h>

//these should be included via the buildsystem
#include "ServerImplLegacy.h"
#include "ServerDescribeImpl.h"
#include "InvokeCapabilityImpl.h"
#include "FetchDataImpl.h"
#include "ServerFlagsImpl.h"
#include "BroadcastNotificationImpl.h"
#include "AuthRequestImpl.h"
#include "AuthRespondImpl.h"
#include "StartSessionImpl.hh"
#include "CloseSessionImpl.hh"
#include "ListSessionsImpl.hh"
#include "KeepAliveSessionImpl.hh"
//#include "GetAuthMechanismsImpl.h"

#include "pushbullet_api.hh"


#include <foxtrot/server/auth_layer/SASLAuthProvider.hh>

#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>

#include <chrono>


using std::string;
using namespace foxtrot;

foxtrot::ServerImplLegacy::ServerImplLegacy(const std::string& servcomment, std::shared_ptr<foxtrot::DeviceHarness> harness)
  : ServerImpl(servcomment, harness, "0.0.0.0:50051"),  _lg("ServerImplLegacy"),
_serverflags{new FlagMap}
{
}

ServerImplLegacy::ServerImplLegacy(const string& servcomment, std::shared_ptr<DeviceHarness> harness, const string& connstr)
  : ServerImpl(servcomment, harness, connstr), _lg("ServerImplLegacy"), _serverflags(new FlagMap)
{

}


void foxtrot::ServerImplLegacy::setup_auth(const std::string& credsfile, int creds_validity_hours)
{
    _lg.strm(sl::debug) << "credsfile: " << credsfile;
    _lg.strm(sl::info) << "registering AuthHandler";
    _auth_api = std::make_shared<AuthHandler>(credsfile, creds_validity_hours);
    
    //_lg.strm(sl::info) << "setting up SASL auth provider";
    //auto saslprov = std::make_shared<SASLAuthProvider>();
    //_lg.strm(sl::info) << "setting up user credentials provider";
    //std::unique_ptr<UserProviderInterface> userprov{nullptr};
    
    //_lg.strm(sl::info) << "setting up user auth interface";
    //_auth_iface = std::make_shared<UserAuthInterface>(std::move(userprov));
    //_auth_iface->add_provider(saslprov);
    
    
    auth_enabled = true;
}





ServerImplLegacy::~ServerImplLegacy()
{
    _server->Shutdown();
    _cq->Shutdown();
    
}

void ServerImplLegacy::setup_common()
{
    common_build(builder);   
    _cq = builder.AddCompletionQueue();
    
    
     auto l1 = create_logic_add_helper<ServerDescribeLogic>(servcomment(),harness());
     auto l2 = create_logic_add_helper<InvokeCapabilityWithSession>(sesman(),harness(), harness());
     auto l3 = create_logic_add_helper<FetchDataLogic>(harness());
     auto l4 = create_logic_add_helper<SetServerFlagsLogic>(_serverflags);
     auto l5 = create_logic_add_helper<GetServerFlagsLogic>(_serverflags);
     auto l6 = create_logic_add_helper<ListServerFlagsLogic>(_serverflags);
     auto l7 = create_logic_add_helper<DropServerFlagLogic>(_serverflags);
     auto l8 = create_logic_add_helper<StartSessionLogic>(sesman());
     auto l9 = create_logic_add_helper<CloseSessionLogic>(sesman());
     auto l10 = create_logic_add_helper<ListSessionsLogic>(sesman());
     auto l11 = create_logic_add_helper<KeepAliveSessionLogic>(sesman());

     auto l12 = create_logic_add_helper<foxtrot::BroadcastNotificationLogic>(steal_noti_api());

     // if(auth_enabled)
     // {
     //      _lg.Info("setting up authentication system");
     //      auto l13 = create_logic_add_helper<AuthRequestLogic>(_auth_api);
     //      auto l14 = create_logic_add_helper<AuthRespondLogic>(_auth_api);
     //      //auto l18 = create_logic_add_helper<GetAuthMechanismsLogic>(_auth_iface);
     // }
     // else
     // {
     //   _lg.Info("authentication system disabled");
     //      auto l15 = create_logic_add_helper<AuthRequestLogic>(nullptr);
     //      auto l16 = create_logic_add_helper<AuthRespondLogic>(nullptr);
     // }

   
    _server = builder.BuildAndStart();
    _lg.Info("server has started...");
}



void ServerImplLegacy::Run()
{
  
    setup_common();
        
    HandleRpcs();
}

std::vector< std::future< std::exception_ptr > > ServerImplLegacy::RunMultithread(int nthreads)
{
  setup_common();
  
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

int ServerImplLegacy::join_multithreaded()
{
  std::unique_lock<std::mutex> lock(_exitmut);
  _lg.Debug("waiting on exit condition...");
  _condvar.wait(lock);
  _lg.Info("thread " + std::to_string(_exitthreadnum) + " exited");
  
  return _exitthreadnum;
}




void foxtrot::ServerImplLegacy::HandleRpcs()
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
	   _lg.Warning("completion queue has a non-OK status!");
         }
        static_cast<HandlerTag*>(tag)->Proceed();
     }
     else
     {
       _lg.Info("completion queue shutdown.");
         
         return;
     }
     
        
    }
}



ServerCompletionQueue* ServerImplLegacy::getCQ()
{
    return _cq.get();
}


