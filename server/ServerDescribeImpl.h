#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"

#include "HandlerBase.h"
#include <iostream>

namespace foxtrot
{
 struct ServerDescribeLogic
 {
  typedef empty reqtp;
  typedef servdescribe repltp;
  constexpr static auto requestfunptr = &exptserve::AsyncService::RequestDescribeServer;
  
  void HandleRequest(reqtp& req, repltp& repl);
  
 };
    
 typedef HandlerBase<ServerDescribeLogic> ServerDescribeImpl;
 
    
}
