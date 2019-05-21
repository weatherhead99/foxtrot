#include <iostream>
#include <foxtrot/server/AuthHandler.h>
#include <sodium.h>
#include <array>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    
//     foxtrot::AuthHandler authhand("./creds.json");

//     auto challenge_string = authhand.get_challenge_string();
    
//     cout << "challenge str: " << challenge_string << endl;
    
    
    std::array<unsigned char,crypto_sign_PUBLICKEYBYTES> pk;
    std::array<unsigned char,crypto_sign_SECRETKEYBYTES> sk;

    auto ret = crypto_sign_keypair(pk.begin(), sk.begin());
    std::cout << "return from keypairgen: " << ret << std::endl;
    
    auto pkstr = detail::bin2base64(pk);

}
