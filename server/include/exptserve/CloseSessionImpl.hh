#pragma once

#include "BaseSessionLogicImpl.hh"

namespace foxtrot
{
    
    struct CloseSessionLogic : public BaseSessionLogicImpl
    {
        constexpr static auto requestfunptr = 
        &sessions::AsyncService::RequestCloseSession;
        
        CloseSessionLogic(std::shared_ptr<SessionManager> sesman);
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    };
    
}
