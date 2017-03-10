#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"

#include "HandlerBase.h"
#include <iostream>
#include <string>
#include "DeviceHarness.h"
#include "Logging.h"

namespace foxtrot
{
 struct ServerDescribeLogic
 {
    ServerDescribeLogic(const std::string& comment, DeviceHarness& harness);
     
  typedef empty reqtp;
  typedef servdescribe repltp;
  constexpr static auto requestfunptr = &exptserve::AsyncService::RequestDescribeServer;
  
  void HandleRequest(reqtp& req, repltp& repl);

  
 private:
  std::string _servcomment;
  DeviceHarness& _harness;
  Logging _lg;
  
 };
    
 typedef HandlerBase<ServerDescribeLogic> ServerDescribeImpl;
 
    
}
