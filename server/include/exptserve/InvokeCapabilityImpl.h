#pragma once
#include <grpc++/grpc++.h>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>

#include <foxtrot/server/DeviceHarness.h>
#include "Logic_defs.hh"

namespace foxtrot
{
    class HandlerTag;
    
    struct InvokeCapabilityLogic : public
    Serverlogic_defs<&exptserve::AsyncService::RequestInvokeCapability>
    {
     InvokeCapabilityLogic(std::shared_ptr<DeviceHarness> harness);
     
     bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
     
    private:
    std::shared_ptr<DeviceHarness> _harness;
	foxtrot::Logging _lg;
    };

}
