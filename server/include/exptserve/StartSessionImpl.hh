#pragma once

#include <foxtrot/ft_sessions.grpc.pb.h>
#include <foxtrot/server/SessionManager.hh>
#include "Logic_defs.hh"

namespace foxtrot 
{
    
    struct StartSessionLogic: 
    public Serverlogic_defs<&sessions::AsyncService::RequestStartSession,
        sessions::AsyncService>
    {

        StartSessionLogic(std::shared_ptr<SessionManager> sesman);
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
    private:
        foxtrot::Logging _lg;
        std::shared_ptr<SessionManager> _sesman;
    };
}

