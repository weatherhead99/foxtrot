#pragma once
#include <iostream>
#include <string>

#include <grpc++/grpc++.h>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>

#include <foxtrot/server/DeviceHarness.h>
#include "Logic_defs.hh"

namespace foxtrot
{
    class HandlerTag;
    
 struct ServerDescribeLogic : public Serverlogic_defs<&exptserve::AsyncService::RequestDescribeServer>
 {
    ServerDescribeLogic(const std::string& comment, std::shared_ptr<DeviceHarness> harness);
    bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);

 private:
  std::string _servcomment;
  std::shared_ptr<DeviceHarness> _harness;
  Logging _lg;
  
 };
    
 
    
}
