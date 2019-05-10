#include <foxtrot/server/auth_utils.h>

using namespace foxtrot;

std::tuple<pkarr, skarr> foxtrot::generate_new_key()
{
  pkarr pkout;
  skarr skout;
  
  //TODO: should we be checking return value of this?
  crypto_sign_keypair(pkout.begin(), skout.begin());
  
  return std::make_tuple(pkout,skout);
};
