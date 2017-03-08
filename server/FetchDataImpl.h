#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "DeviceHarness.h"

namespace foxtrot
{
    struct FetchDataImpl
    {
        
        FetchDataImpl(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq);
        
        
        void HandleRequest(reqtp& req, repltp& repl);
        
    private:
        exptserve::AsyncService* _service;
        
    };
    
    
    

    
    
}
