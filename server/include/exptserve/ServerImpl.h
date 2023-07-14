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

using std::string;

namespace foxtrot{
    using ft_std_variant = std::variant<double,int,bool,std::string>;
    using flagmap =  std::map<std::string, ft_std_variant> ;


  class ServerImpl{

  };
  
}
