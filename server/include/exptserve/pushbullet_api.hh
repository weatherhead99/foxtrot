#pragma once
#include <string>
#include <memory>
#include <optional>

#include <foxtrot/Logging.h>

#include <foxtrot/protocols/curlRequest.h>

using std::string;

using optstring = std::optional<std::reference_wrapper<string>>;


namespace foxtrot {
 
    class pushbullet_api {
      
    public:
      [[deprecated]] pushbullet_api(const string& api_key);
      pushbullet_api(const string& api_key, const string& default_channel,
		     const string& default_title);

      
      void push_to_channel(const string& title, const string& body, const string& channel);

      void push_to_channel_with_defaults(const string& body, optstring title=std::nullopt, optstring channel=std::nullopt);

      
        
    private:
        string api_key_;
        std::shared_ptr<foxtrot::protocols::CurlRequest> proto_;
        Logging lg_;

      std::optional<string>  _default_title;
      std::optional<string> _default_channel;

      
    };
    
    string get_pushbullet_authkey(const string& client_id);
    
    
}
