#pragma once
#include "HandlerBase.h"
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/Logging.h>
#include "Logic_defs.hh"
#include <foxtrot/ft_auth.grpc.pb.h>

namespace foxtrot {
    struct AuthRespondLogic : 
    public Serverlogic_defs<&auth::AsyncService::RequestRespondAuthChallenge,
        auth::AsyncService>
    {
        
        AuthRespondLogic(std::shared_ptr<AuthHandler> authhand);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    private:
        std::shared_ptr<AuthHandler> authhand_;
        Logging lg_;
    };
}
