#include <string>
#include <fstream>
#include <sstream>
//might be needed on windows?
#include <exception>
#include <iostream>
#include <typeinfo>

#include <foxtrot/server/AuthHandler.h>

//these should be included via the buildsystem
#include "ServerImpl.h"
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


#include <foxtrot/server/auth_layer/SASLAuthProvider.hh>

#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>

#include <chrono>


using std::string;
using namespace foxtrot;

foxtrot::ServerImpl::ServerImpl(const std::string& servcomment, std::shared_ptr<foxtrot::DeviceHarness> harness)
: _servcomment(servcomment), _harness(harness), _lg("ServerImpl"), _connstr("0.0.0.0:50051"),
_serverflags{new FlagMap}
{
    //TODO: option for length of session
    auto session_length = std::chrono::minutes(10);
    _sesman = std::make_shared<SessionManager>(session_length);
}

ServerImpl::ServerImpl(const string& servcomment, std::shared_ptr<DeviceHarness> harness, const string& connstr)
: ServerImpl(servcomment,harness)
{
  _connstr = connstr;

}

void foxtrot::ServerImpl::setup_notifications(const string& apikey, const string& default_title,
                                              const string& default_channel)
{
    _lg.strm(sl::info) << "API key: " << apikey;
    _noti_api = std::make_unique<pushbullet_api>(apikey);
    notifications_enabled = true;
    default_channel_ = default_channel;
    default_title_ = default_title;
}

void foxtrot::ServerImpl::setup_auth(const std::string& credsfile, int creds_validity_hours)
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





ServerImpl::~ServerImpl()
{
    _server->Shutdown();
    _cq->Shutdown();
    
}

void ServerImpl::setup_common(const std::string& addrstr)
{
  
    if(_creds == nullptr)
    {
      _creds = grpc::InsecureServerCredentials();
      
    };
  
    builder.AddListeningPort(addrstr,_creds);
    _cq = builder.AddCompletionQueue();
    
    std::vector<std::unique_ptr<logic_add_helper_base>> logics;
    
    logics.push_back( create_logic_add_helper<ServerDescribeLogic>(_servcomment,_harness));
    logics.push_back( create_logic_add_helper<InvokeCapabilityWithSession>(_sesman, _harness, _harness));
    logics.push_back( create_logic_add_helper<FetchDataLogic>(_harness));
    logics.push_back( create_logic_add_helper<SetServerFlagsLogic>(_serverflags));
    logics.push_back( create_logic_add_helper<GetServerFlagsLogic>(_serverflags));
    logics.push_back( create_logic_add_helper<ListServerFlagsLogic>(_serverflags));
    logics.push_back( create_logic_add_helper<DropServerFlagLogic>(_serverflags));
    logics.push_back( create_logic_add_helper<StartSessionLogic>(_sesman));
    logics.push_back( create_logic_add_helper<CloseSessionLogic>(_sesman));
    logics.push_back( create_logic_add_helper<ListSessionsLogic>(_sesman));
    logics.push_back( create_logic_add_helper<KeepAliveSessionLogic>(_sesman));
    if(notifications_enabled)
    {
        _lg.Info("setting up pushbullet notification logic");
        logics.push_back(create_logic_add_helper<BroadcastNotificationLogic>(std::move(_noti_api),
                                                                             default_title_, default_channel_));
    }
    else
    {
        _lg.Info("notifications are not enabled");
//         logics.push_back<BroadcastNotificationLogic>(nullptr);
    }

    if(auth_enabled)
    {
        _lg.Info("setting up authentication system");
        logics.push_back(create_logic_add_helper<AuthRequestLogic>(_auth_api));
        logics.push_back(create_logic_add_helper<AuthRespondLogic>(_auth_api));
        //logics.push_back(create_logic_add_helper<GetAuthMechanismsLogic>(_auth_iface));
    }
    else
    {
        _lg.Info("authentication system disabled");
        logics.push_back(create_logic_add_helper<AuthRequestLogic>(nullptr));
        logics.push_back(create_logic_add_helper<AuthRespondLogic>(nullptr));
    }

    _server = builder.BuildAndStart();
    

    
    _lg.Info("server listening on " + addrstr );
    
}



void ServerImpl::Run()
{
    setup_common(_connstr);
        
    HandleRpcs();
}

std::vector< std::future< std::exception_ptr > > ServerImpl::RunMultithread(int nthreads)
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


void ServerImpl::SetupSSL(const string& crtfile, const string& keyfile, bool force_client_auth)
{
  grpc::SslServerCredentialsOptions::PemKeyCertPair kp;
  
  std::ifstream ifs(crtfile);
  std::stringstream iss;
  
  iss << ifs.rdbuf();
  
  kp.cert_chain = iss.str();
  ifs.close();
  ifs.clear();
  
  
  ifs.open(keyfile);
  iss.str("");
  iss << ifs.rdbuf();
  
  kp.private_key = iss.str();
  
  grpc::SslServerCredentialsOptions opts;
  opts.pem_key_cert_pairs.push_back(kp);
  
  opts.force_client_auth = force_client_auth;
  
  
  _creds = grpc::SslServerCredentials(opts);
  
}

ServerCompletionQueue* ServerImpl::getCQ()
{
    return _cq.get();
}


