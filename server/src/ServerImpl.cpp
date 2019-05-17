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

using std::string;
using namespace foxtrot;

foxtrot::ServerImpl::ServerImpl(const std::string& servcomment, foxtrot::DeviceHarness& harness)
: _servcomment(servcomment), _harness(harness), _lg("ServerImpl"), _connstr("0.0.0.0:50051"),
_serverflags{new flagmap}
{
}

ServerImpl::ServerImpl(const string& servcomment, DeviceHarness& harness, const string& connstr)
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
    _cred_validity_hours = creds_validity_hours;
    _lg.strm(sl::info) << "registering AuthHandler";
    _auth_api = std::make_shared<AuthHandler>(credsfile);
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
  
  
    ServerBuilder builder;
    //TODO: SECURE CREDENTIALS!
    builder.AddListeningPort(addrstr,_creds);
    
    //TODO: Register Service
    builder.RegisterService(&_service);
    
    _cq = builder.AddCompletionQueue();
    _server = builder.BuildAndStart();
    _lg.Info("server listening on " + addrstr );
    add_logic<ServerDescribeLogic>(_servcomment,_harness);
    add_logic<InvokeCapabilityLogic>(_harness);
    add_logic<FetchDataLogic>(_harness);
    add_logic<SetServerFlagsLogic>(_serverflags);
    add_logic<GetServerFlagsLogic>(_serverflags);
    add_logic<ListServerFlagsLogic>(_serverflags);
    add_logic<DropServerFlagLogic>(_serverflags);
    
    if(notifications_enabled)
    {
        _lg.Info("setting up pushbullet notification logic");
        add_logic<BroadcastNotificationLogic>(std::move(_noti_api),default_title_,
            default_channel_);
    }
    else
    {
        _lg.Info("notifications are not enabled");
        add_logic<BroadcastNotificationLogic>(nullptr);
    }
    if(auth_enabled)
    {
        _lg.Info("setting up authentication system");
        add_logic<AuthRequestLogic>(_auth_api);
    }
    else
    {
        _lg.Info("authentication system disabled");
        add_logic<AuthRequestLogic>(nullptr);
    }
    
    
    
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




