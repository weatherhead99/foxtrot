#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"


namespace foxtrot
{
    class CapabilityRequestImpl : public foxtrot::exptserve::AsyncService
    {
        
        grpc::Status AsyncInvokeCapability() override;
        
    };
    
    
}
