#pragma once
#include <grpc++/grpc++.h>
#include "HandlerBase.h"
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/Logging.h>
#include "Logic_defs.hh"

namespace foxtrot {
    struct AuthRequestLogic : public Serverlogic_defs<&exptserve::AsyncService::RequestRequestAuthChallenge>
    {
        AuthRequestLogic(std::shared_ptr<AuthHandler> authhand);
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
    private:
        std::shared_ptr<AuthHandler> authhand_;
        Logging lg_;
    };
}
