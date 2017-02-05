#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"

namespace foxtrot
{
    struct InvokeCapabilityLogic
    {
     typedef capability_request reqtp;
     typedef capability_response repltp;
     constexpr static auto requestfunptr = &exptserve::AsyncService::RequestInvokeCapability;
     
     void HandleRequest(reqtp& req, repltp& repl);
     
    };
    
    typedef HandlerBase<InvokeCapabilityLogic> InvokeCapabilityImpl;

}
