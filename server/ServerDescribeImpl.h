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
  typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
  
  constexpr static auto requestfunptr = &exptserve::AsyncService::RequestDescribeServer;
  
  bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, void* tag);

  
 private:
  std::string _servcomment;
  DeviceHarness& _harness;
  Logging _lg;
  
 };
    
 typedef HandlerBase<ServerDescribeLogic> ServerDescribeImpl;
 
    
}
