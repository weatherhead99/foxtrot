#pragma once
#include <map>
#include <string>
#include <memory>

#include <variant>
#include <grpc++/grpc++.h>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>


namespace foxtrot {
    
    class HandlerTag;
  
  using ft_std_variant = std::variant<double,int,bool,std::string>;
  using flagmap =  std::map<std::string, ft_std_variant> ;
  
  
  struct SetServerFlagsLogic
  {
    typedef serverflag reqtp;
    typedef serverflag repltp;
    typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
    const static bool newcall = true;
    constexpr static auto requestfunptr = &exptserve::AsyncService::RequestSetServerFlag;
    
    SetServerFlagsLogic(std::shared_ptr<flagmap> vars);
    
    bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
    
  private:
    std::shared_ptr<flagmap> vars_;
    foxtrot::Logging lg_;
    
  };
  
  
  struct GetServerFlagsLogic
  {
    typedef serverflag reqtp;
    typedef serverflag repltp;
    typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
    const static bool newcall = true;
    constexpr static auto requestfunptr = &exptserve::AsyncService::RequestGetServerFlag;
    
    GetServerFlagsLogic(std::shared_ptr<flagmap> vars);
    bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
    
  private:
    std::shared_ptr<flagmap> vars_;
    foxtrot::Logging lg_;
    
  };
  
  

  struct ListServerFlagsLogic
  {
      typedef empty reqtp;
      typedef serverflaglist repltp;
      typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
      
      const static bool newcall = true;
      constexpr static auto requestfunptr = &exptserve::AsyncService::RequestListServerFlags;
      
      ListServerFlagsLogic(std::shared_ptr<flagmap> vars);
      bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
      
  private:
      std::shared_ptr<flagmap> vars_;
      foxtrot::Logging lg_;
      
  };
  
  struct DropServerFlagLogic
  {
      typedef serverflag reqtp;
      typedef serverflag repltp;
      
      typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
      const static bool newcall = true;
      
      constexpr static auto requestfunptr = &exptserve::AsyncService::RequestDropServerFlag;
      DropServerFlagLogic(std::shared_ptr<flagmap> vars);
      bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
      
  private:
      std::shared_ptr<flagmap> vars_;
      foxtrot::Logging lg_;
      
          
  };
  
}
