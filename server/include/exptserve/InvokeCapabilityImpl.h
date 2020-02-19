#pragma once
#include <grpc++/grpc++.h>

#include <foxtrot/ft_capability.grpc.pb.h>
#include <foxtrot/Logging.h>

#include <foxtrot/server/DeviceHarness.h>
#include "Logic_defs.hh"
#include <foxtrot/server/SessionHandlerMixin.hh>

namespace foxtrot
{
    
    class HandlerTag;
    
    struct InvokeCapabilityLogic : public
    Serverlogic_defs<&capability::AsyncService::RequestInvokeCapability>
    {
     InvokeCapabilityLogic(std::shared_ptr<DeviceHarness> harness);
     
     bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
     
    private:
    std::shared_ptr<DeviceHarness> _harness;
	foxtrot::Logging _lg;
    };
    

    using InvokeCapabilityWithSession = SessionHandlerMixin<InvokeCapabilityLogic>;

}
