#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "DeviceHarness.h"
#include "Logging.h"

#include <map>

namespace foxtrot
{
    struct FetchDataLogic
    {
        typedef chunk_request reqtp;
        typedef datachunk repltp; 
	typedef grpc::ServerAsyncWriter<datachunk> respondertp;
	
        constexpr static auto requestfunptr = &exptserve::AsyncService::RequestFetchData;
        
        FetchDataLogic(DeviceHarness& harness);
        
	
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, void* tag);
        
	
    private:
        DeviceHarness& _harness;
	foxtrot::Logging _lg;
        
    };
    
    
    typedef HandlerBase<FetchDataLogic> FetchDataImpl;

    
    
}
