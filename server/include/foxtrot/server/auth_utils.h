#pragma once

#include <array>
#include <tuple>
#include <vector>
#include <sodium.h>
#include <string>
#include <stdexcept>

const int CHALLENGE_STRING_BYTES  = 32;
const int SESSIONKEY_BYTES = 64;

namespace foxtrot {
    using pkarr = std::array<unsigned char, crypto_sign_PUBLICKEYBYTES>;
    using skarr = std::array<unsigned char, crypto_sign_SECRETKEYBYTES>;
    using seskeyarr = std::array<unsigned char, SESSIONKEY_BYTES>;
    using sigarr = std::array<unsigned char, crypto_sign_BYTES>;
    
    using challengearr = std::array<unsigned char, CHALLENGE_STRING_BYTES>;
    using keypair = std::tuple<pkarr,skarr>;
    
    keypair generate_new_key();
    void save_creds_to_file(const std::string& fname, const std::string& userid,
                            const keypair& keys);
    
    keypair get_creds_from_client_file(const std::string& fname, const std::string& userid);
    
    void auth_user_to_file(const std::string& fname, const std::string& userid, const pkarr& pk,
                           int authlevel, const std::string& keyname="default");
    
    void export_pubkey(const std::string& outfname, const std::string& userid, const pkarr& pk,
                       const std::string& keyname="default");
    
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
