#include "AuthRequestImpl.h"


using namespace foxtrot;

AuthRequestLogic::AuthRequestLogic(std::shared_ptr<AuthHandler> authhand)
: authhand_(authhand), lg_("AuthRequestLogic")
{}

bool AuthRequestLogic::HandleRequest(AuthRequestLogic::reqtp& req, AuthRequestLogic::repltp& repl, AuthRequestLogic::respondertp& respond, HandlerTag* tag)
{
    //TODO: refuse if channel does not have TLS enabled!
    
}
