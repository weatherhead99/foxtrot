#pragma once
#include <string>
#include <map>
#include <vector>
#include <array>

#include <foxtrot/Logging.h>
#include <foxtrot/server/auth_utils.h>
#include <sodium.h>



namespace foxtrot {
    
    using credentialsmap = std::map<std::string, std::vector<std::pair<std::string,foxtrot::pkarr>>>;
    
    class AuthHandler{
    public:
        AuthHandler(const std::string& filename);
        AuthHandler(credentialsmap&& creds);
        
        std::string get_challenge_string() const;
        std::string authenticate_user(const std::string& userid, const std::string& response, int valid_seconds) const;
        int get_privilege_level(const std::string& userid) const;
        int validate_session_key(const std::string& key) const;
    private:
        credentialsmap load_creds_from_file(const std::string& filename);
        //TODO: this should be mlock'd for security
        credentialsmap _creds;
        std::map<std::string, time_t> _sessionkeys;
        Logging _lg;
    };
}

