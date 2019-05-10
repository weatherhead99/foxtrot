#pragma once

#include <array>
#include <tuple>
#include <sodium.h>

namespace foxtrot {
    using pkarr = std::array<unsigned char, crypto_sign_PUBLICKEYBYTES>;
    using skarr = std::array<unsigned char, crypto_sign_SECRETKEYBYTES>;
    
    
    std::tuple<pkarr, skarr> generate_new_key();

}
