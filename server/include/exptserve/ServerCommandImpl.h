#pragma once
#include <grpc++/grpc++.h>

#include <foxtrot/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>

#include <foxtrot/server/ExperimentalSetup.h>

#include "HandlerBase.h"

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
    
    
