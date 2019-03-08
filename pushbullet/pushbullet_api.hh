#pragma once
#include <string>
#include <memory>
#include "protocols/curlRequest.h"
#include "Logging.h"

using std::string;



namespace foxtrot {
 
    class pushbullet_api {
      
    public:
        pushbullet_api(const string&& api_key);                                                                                                                                                                                                                                                                                                           
        void create_push_channel(const string& title, const string& body, const string& channel);
    private:
        string api_key_;
        std::shared_ptr<foxtrot::protocols::CurlRequest> proto_;
        Logging lg_;
    };
    
}
