#pragma once

#include "Logic_defs.hh"
#include <proto/ft_sessions.grpc.pb.h>
#include <foxtrot/server/SessionManager.hh>

namespace foxtrot
{
    
    struct CloseSessionLogic :
    public Serverlogic_defs<&sessions::AsyncService::RequestCloseSession,
        sessions::AsyncService>
    {

        CloseSessionLogic(std::shared_ptr<SessionManager> sesman);
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);

    private:
        foxtrot::Logging _lg;
        std::shared_ptr<foxtrot::SessionManager> _sesman;
    };
    
}
