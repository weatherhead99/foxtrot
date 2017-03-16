#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "DeviceHarness.h"
#include "Logging.h"

namespace foxtrot
{
    struct InvokeCapabilityLogic
    {
     typedef capability_request reqtp;
     typedef capability_response repltp;
     constexpr static auto requestfunptr = &exptserve::AsyncService::RequestInvokeCapability;
     
     InvokeCapabilityLogic(DeviceHarness& harness);
     
     void HandleRequest(reqtp& req, repltp& repl);
     
    private:
        DeviceHarness& _harness;
	foxtrot::Logging _lg;
    };
    
    typedef HandlerBase<InvokeCapabilityLogic> InvokeCapabilityImpl;

}
