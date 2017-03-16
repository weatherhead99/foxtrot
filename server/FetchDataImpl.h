#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "DeviceHarness.h"
#include "Logging.h"

namespace foxtrot
{
    struct FetchDataLogic
    {
        typedef chunk_request reqtp;
        typedef grpc::ServerWriter<datachunk> repltp; 
        constexpr static auto requestfunptr = &exptserve::AsyncService::RequestFetchData;
        
        FetchDataLogic(DeviceHarness& harness);
        
        void HandleRequest(reqtp& req, repltp& writer);
        
    private:
        DeviceHarness& _harness;
	foxtrot::Logging _lg;
        
    };
    
    
    typedef HandlerBase<FetchDataLogic> FetchDataImpl;

    
    
}
