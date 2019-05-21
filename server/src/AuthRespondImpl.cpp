#include "AuthRespondImpl.h"

using namespace foxtrot;


AuthRespondLogic::AuthRespondLogic(std::shared_ptr<AuthHandler> authhand)
: authhand_(authhand), lg_("AuthRespondLogic")
{
    
}


bool foxtrot::AuthRespondLogic::HandleRequest(foxtrot::AuthRespondLogic::reqtp& req, foxtrot::AuthRespondLogic::repltp& repl, foxtrot::AuthRespondLogic::respondertp& respond, foxtrot::HandlerTag* tag)
{
    if(!authhand_)
    {
        
        foxtrot_server_specific_error("authentication not configured on this server",
            repl, respond, lg_, tag, error_types::ft_AuthError);
        
        return true;
    }
    
    //TODO: refuse to do it if the channel isn't TLS secure!
 
    int authlevel;
    seskeyarr sessionkey;
    sigarr sig;
    std::copy(req.sig().begin(), req.sig().begin() + sig.size(), sig.begin());
    
    auto success = authhand_->verify_response(req.userid(), req.challengeid(), sig, authlevel, sessionkey);
    
    
 
}
