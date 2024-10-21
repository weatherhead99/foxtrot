#pragma once

#include <proto/ft_sessions.grpc.pb.h>
#include <foxtrot/server/SessionManager.hh>

namespace foxtrot 
{
    class HandlerTag;
    struct BaseSessionLogicImpl
    {
        typedef session_info reqtp;
        typedef session_info repltp;

        typedef grpc::ServerAsyncResponseWriter<repltp> respondertp;
        const static bool newcall = true;
        
        BaseSessionLogicImpl(const std::string& logname, std::shared_ptr<SessionManager> sesman): _sesman(sesman), _lg(logname){};
        
        protected:
            foxtrot::Logging _lg;
            std::shared_ptr<SessionManager> _sesman;
    };
    
}
