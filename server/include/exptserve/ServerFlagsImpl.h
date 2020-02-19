#pragma once
#include <map>
#include <string>
#include <memory>

#include <variant>
#include <grpc++/grpc++.h>

#include <foxtrot/ft_flags.grpc.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/server/FlagMap.hh>
#include "Logic_defs.hh"

namespace foxtrot {
    
    class HandlerTag;

  
  struct SetServerFlagsLogic : 
  public Serverlogic_defs<&flags::AsyncService::RequestSetServerFlag>
  {
    SetServerFlagsLogic(std::shared_ptr<FlagMap> vars);
    bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
  private:
    std::shared_ptr<FlagMap> vars_;
    foxtrot::Logging lg_;
    
  };
  
  
  struct GetServerFlagsLogic : 
  public Serverlogic_defs<&flags::AsyncService::RequestGetServerFlag>
  {
    GetServerFlagsLogic(std::shared_ptr<FlagMap> vars);
    bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
    
  private:
    std::shared_ptr<FlagMap> vars_;
    foxtrot::Logging lg_;
    
  };
  
  

  struct ListServerFlagsLogic : 
  public Serverlogic_defs<&flags::AsyncService::RequestListServerFlags>
  {

      ListServerFlagsLogic(std::shared_ptr<FlagMap> vars);
      bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);

  private:
      std::shared_ptr<FlagMap> vars_;
      foxtrot::Logging lg_;
      
  };
  
  struct DropServerFlagLogic:
  public Serverlogic_defs<&flags::AsyncService::RequestDropServerFlag>
  {
      DropServerFlagLogic(std::shared_ptr<FlagMap> vars);
      bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
  private:
      std::shared_ptr<FlagMap> vars_;
      foxtrot::Logging lg_;
      
          
  };
  
}
