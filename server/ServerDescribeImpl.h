#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"

#include "HandlerBase.h"
#include <iostream>
#include <string>

namespace foxtrot
{
 struct ServerDescribeLogic
 {
  typedef empty reqtp;
  typedef servdescribe repltp;
  constexpr static auto requestfunptr = &exptserve::AsyncService::RequestDescribeServer;
  
  void HandleRequest(reqtp& req, repltp& repl);
  
  std::string _servcomment;
  
 };
    
 typedef HandlerBase<ServerDescribeLogic> ServerDescribeImpl;
 
    
}
