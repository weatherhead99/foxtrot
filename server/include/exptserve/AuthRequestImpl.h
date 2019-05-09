#pragma once
#include <grpc++/grpc++.h>
#include "HandlerBase.h"
#include <foxtrot/server/AuthHandler.h>
#include <foxtrot/Logging.h>

namespace foxtrot {
    struct AuthRequestLogic
    {
        typedef empty reqtp;
        typedef auth_challenge repltp;
        typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
        constexpr static auto requestfunptrf = &exptserve::AsyncService::RequestRequestAuthChallenge;
        
        AuthRequestLogic(std::shared_ptr<AuthHandler> authhand);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    private:
        std::shared_ptr<AuthHandler> authhand_;
        Logging lg_;
    };
}
