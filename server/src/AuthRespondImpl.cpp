#include "AuthRespondImpl.h"

using namespace foxtrot;


AuthRespondLogic::AuthRespondLogic(std::shared_ptr<AuthHandler> authhand)
: authhand_(authhand), lg_("AuthRespondLogic")
{
    
}
