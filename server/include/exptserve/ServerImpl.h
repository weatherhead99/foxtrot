#pragma once
#include <memory>
#include <string>
#include <future>
#include <exception>
#include <condition_variable>
#include <mutex>
#include <map>
#include <typeinfo>

#include <grpc++/grpc++.h>
#include <grpc++/security/credentials.h>

#include <variant>

#include <foxtrot/foxtrot.pb.h>
#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/ft_sessions.grpc.pb.h>
#include <foxtrot/ft_capability.grpc.pb.h>
#include <foxtrot/ft_auth.grpc.pb.h>
#include <foxtrot/ft_flags.grpc.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/DeviceHarness.h>
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/server/FlagMap.hh>
#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/server/auth_layer/AuthBase.hh>

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerBuilder;

using std::shared_ptr;
using std::string;
using std::unique_ptr;

namespace foxtrot{
    using ft_std_variant = std::variant<double,int,bool,std::string>;
    using flagmap =  std::map<std::string, ft_std_variant> ;


  struct _ServerImplInternal;
  struct pushbullet_api;

  class ServerImpl{
  public:
    ServerImpl(const string& servcomment, std::shared_ptr<DeviceHarness> harness,
	       const string& connstr);

    ~ServerImpl();
    
    void setup_notifications(const string& apikey, const string& default_title, const string& default_channel);

    void SetupSSL(const string& crtfile, const string& keyfile,
		  bool force_client_auth = false);


  protected:
    unique_ptr<pushbullet_api> steal_noti_api();
    
  private:
    foxtrot::Logging _lg;
    unique_ptr<_ServerImplInternal> _impl;

  };
  
}
