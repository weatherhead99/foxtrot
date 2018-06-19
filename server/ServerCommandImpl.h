#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "ExperimentalSetup.h"
#include "Logging.h"

namespace foxtrot
{
    struct ServerCommandLogic
    {
        typedef servercommand_request reqtp;
        typedef serverrequest_return repltp;
        typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
        
        ServerCommandLogic(ExperimentalSetup& setup);
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
      
    private:
//         void ReloadSetup(repltp& repl);
        void ChangeDebugLevel(const reqtp& req, repltp& repl);
        
    private:
        Logging _lg;
        ExperimentalSetup& _setup;
        
    };
    
    
}
    
    
