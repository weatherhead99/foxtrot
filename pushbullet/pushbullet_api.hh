#pragma once
#include <string>
#include <memory>
#include "protocols/curlRequest.h"
#include "Logging.h"

using std::string;



namespace foxtrot {
 
    class pushbullet_api {
      
    public:
        pushbullet_api(const string&& api_key)
        
    private:
        string api_key_;
        std::shared_ptr<foxtrot::curlRequest> proto_;
        Logging lg_;
    };
    
}
