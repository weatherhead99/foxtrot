#pragma once
#include <string>
#include "protocols/curlRequest.h"

namespace foxtrot {
 
    
    class pushbullet_client {
    public:
        pushbullet_client(const std::string& authkey);
        void new_channel_push(const string& channel, const string& title, const string& body);
        
        
    private:
        std::string authkey_;
        
    };
    
    
    
}
