#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "Logging.h"

#include <boost/variant.hpp>
#include <map>
#include <string>
#include <memory>

namespace foxtrot {
  
  typedef boost::variant<double,int,bool,std::string> ft_variant;
  
  using flagmap =  std::map<std::string, ft_variant> ;
  
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
  
  typedef HandlerBase<SetServerFlagsLogic> SetServerFlagsImpl;
  
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
  
  typedef HandlerBase<GetServerFlagsLogic> GetServerFlagsImpl;
  

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
  
  typedef HandlerBase<ListServerFlagsLogic> ListServerFlagsImpl;
}
