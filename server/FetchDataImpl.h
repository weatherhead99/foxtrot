#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "DeviceHarness.h"

namespace foxtrot
{
    struct FetchDataLogic
    {
        typedef capability_request reqtp;
        typedef grpc::ServerWriter<datachunk> repltp; 
        constexpr static auto requestfunptr = &exptserve::AsyncService::RequestFetchData;
        
        FetchDataLogic(DeviceHarness& harness);
        
        void HandleRequest(reqtp& req, repltp& writer);
        
    private:
        DeviceHarness& _harness;
        
    };
    
    
    typedef HandlerBase<FetchDataLogic> FetchDataImpl;

    
    
}
