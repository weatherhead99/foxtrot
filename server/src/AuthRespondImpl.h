#pragma once
#include <grpc++/grpc++.h>
#include "HandlerBase.h"
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/Logging.h>

namespace foxtrot {
    struct AuthRespondLogic
    {
        typedef auth_response reqtp;
        typedef auth_confirm repltp;
        typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
        constexpr static auto requestfunptr = &exptserve::AsyncService::RequestRespondAuthChallenge;
        
        AuthRespondLogic(std::shared_ptr<AuthHandler> authhand);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    private:
        std::shared_ptr<AuthHandler> authhand_;
        Logging lg_;
    };
}
