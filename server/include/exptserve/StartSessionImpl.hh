#pragma once

#include <foxtrot/ft_sessions.grpc.pb.h>
#include <foxtrot/server/SessionManager.hh>

namespace foxtrot 
{
    class HandlerTag;
    
    struct StartSessionLogic
    {
        constexpr static auto requestfunptr = 
        &sessions::AsyncService::RequestStartSession;
        
        typedef session_info reqtp;
        typedef session_info repltp;
        
        typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
        const static bool newcall = true;
        
        StartSessionLogic(std::shared_ptr<SessionManager> sesman);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    private:
        foxtrot::Logging _lg;
        std::shared_ptr<SessionManager> _sesman;
    };
}

