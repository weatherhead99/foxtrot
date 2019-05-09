#pragma once
#include <string>
#include <map>
#include <vector>
#include <array>

#include <foxtrot/Logging.h>
#include <sodium.h>

const int CHALLENGE_STRING_BYTES  = 32;

namespace foxtrot {
    
    using credentialsmap = std::map<std::string, std::vector<std::string>>;
    
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

namespace detail
{
    std::array<unsigned char, CHALLENGE_STRING_BYTES> get_challenge_bytes();
    
    template<typename arrtp> std::string
    bin2base64(const arrtp& arr)
    {
        auto len = sodium_base64_encoded_len(arr.size(),
                                         sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
        std::string out(len-1,0);
        auto ret = sodium_bin2base64(const_cast<char*>(out.data()),len, arr.data(),arr.size(),
        sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
        if(ret == nullptr)
        {
            throw std::runtime_error("failed to base64 encode string!");
        }
        return out;
    };
    
    
    std::vector<unsigned char> base642bin(const std::string& base64str);
    
}
