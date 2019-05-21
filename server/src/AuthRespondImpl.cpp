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
    time_t expiry;
    
    auto success = authhand_->verify_response(req.userid(), req.challengeid(), sig, authlevel, expiry, sessionkey);
    
    
    if(!success)
    {
        foxtrot_server_specific_error("failed to authenticate user", repl, respond, lg_, tag, error_types::ft_AuthError);
        return true;
    }
    
    repl.mutable_sessionkey()->assign(sessionkey.begin(),sessionkey.end());
    repl.set_authlevel(authlevel);
    repl.set_expiry(expiry);
    
    respond.Finish(repl, grpc::Status::OK, tag);
    
    return true;
 
}
