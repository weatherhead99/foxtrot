#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <array>
#include <deque>

#include <foxtrot/Logging.h>
#include <foxtrot/server/auth_utils.h>
#include <sodium.h>
#include <mutex>

const unsigned MAX_CHALLENGES = 4096;

namespace foxtrot {
    
    using key_info = std::pair<std::string,foxtrot::pkarr>;
    using login_info = std::tuple<std::string,int, time_t>;
    using credentialsmap = std::map<std::string, std::pair<std::vector<key_info>,int>>;
    using matchfiltertuple = std::tuple<std::string, int, bool>;

    class AuthHandler{
    public:
        AuthHandler(const std::string& filename, int valid_hours);
        AuthHandler(credentialsmap&& creds, int valid_hours);
        
        
        std::pair<std::string,unsigned> get_challenge_string(const std::string& userid);
        std::pair<challengearr, unsigned>
        get_challenge_binary(const std::string& userid);
        bool verify_response(const std::string& userid, unsigned challenge_id,
            const sigarr& sig, int& authlevel, time_t& expiry, seskeyarr& sessionkey);
        

    private:
        credentialsmap load_creds_from_file(const std::string& filename);
        //TODO: this should be mlock'd for security
        credentialsmap _creds;
        std::map<unsigned, std::pair<std::string,challengearr>> _challenges;
        std::map<seskeyarr, login_info> _sessionkeys;
        std::deque<unsigned> _challenge_order;
        Logging _lg;
        
        std::mutex challenge_mut;
        std::mutex session_mut;
        int valid_hours_;
    };
}

