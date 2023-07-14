#pragma once
#include <memory>

#include <grpc++/grpc++.h>

#include <foxtrot/Logging.h>


#include "HandlerBase.h"
#include "Logic_defs.hh"

using std::string;

namespace foxtrot 
{

  class pushbullet_api;
    
    
    struct BroadcastNotificationLogic
    : public Serverlogic_defs<&exptserve::AsyncService::RequestBroadcastNotification,
    exptserve::AsyncService>
    {
        BroadcastNotificationLogic(std::unique_ptr<pushbullet_api> api, 
            const string& default_title="",
            const string& default_channel=""
        );
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                           HandlerTag* tag);
        
    private:
        std::unique_ptr<pushbullet_api> api_;
        Logging lg_;
        string default_title_;
        string default_channel_;


    };
}
