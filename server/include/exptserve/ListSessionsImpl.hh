#pragma once

#include "Logic_defs.hh"
#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/ft_sessions.grpc.pb.h>


namespace foxtrot
{
    class ListSessionsLogic 
    : public Serverlogic_defs<&sessions::AsyncService::RequestListSessions,
    sessions::AsyncService>
    {
    public:
        ListSessionsLogic(std::shared_ptr<SessionManager> sesman);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    private:
        foxtrot::Logging _lg;
        std::shared_ptr<SessionManager> _sesman;
    };
    
}
