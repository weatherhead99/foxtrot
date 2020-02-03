#pragma once
#include "Logic_defs.hh"
#include <foxtrot/ft_sessions.grpc.pb.h>
#include <foxtrot/server/SessionManager.hh>

namespace foxtrot
{
    
    class KeepAliveSessionLogic : 
    public Serverlogic_defs<&sessions::AsyncService::RequestKeepAliveSession>
    {
    public:
        KeepAliveSessionLogic(std::shared_ptr<SessionManager> sesman);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& repsond,
                           HandlerTag* tag);
        
    private:
        foxtrot::Logging _lg;
        std::shared_ptr<SessionManager> _sesman;
    };
    
}
