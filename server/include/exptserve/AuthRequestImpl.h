#pragma once
#include <foxtrot/ft_auth.grpc.pb.h>
#include "HandlerBase.h"
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/Logging.h>
#include "Logic_defs.hh"

namespace foxtrot {
    struct AuthRequestLogic : public Serverlogic_defs<&auth::AsyncService::RequestRequestAuthChallenge,
    auth::AsyncService>
    {
        AuthRequestLogic(std::shared_ptr<AuthHandler> authhand);
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
    private:
        std::shared_ptr<AuthHandler> authhand_;
        Logging lg_;
    };
}
