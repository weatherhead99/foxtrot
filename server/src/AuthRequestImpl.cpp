#include "AuthRequestImpl.h"


using namespace foxtrot;

AuthRequestLogic::AuthRequestLogic(std::shared_ptr<AuthHandler> authhand)
: authhand_(authhand), lg_("AuthRequestLogic")
{}

bool AuthRequestLogic::HandleRequest(AuthRequestLogic::reqtp& req, AuthRequestLogic::repltp& repl, AuthRequestLogic::respondertp& respond, HandlerTag* tag)
{
    
    lg_.strm(sl::trace) << "in handlerequest for authrequest";
    
    if(!authhand_)
    {
        foxtrot_server_specific_error("authentication not configured on this server",
            repl, respond, lg_, tag, error_types::ft_AuthError);
        
        return true;
    }
    
    
    lg_.strm(sl::trace) << "checked authhandler existence";
    //TODO: refuse if channel does not have TLS enabled!
    
    auto& servctxt = static_cast<HandlerBase<AuthRequestLogic>*>(tag)->getContext();
    
    lg_.strm(sl::trace) << "created server context";
    auto isauth = servctxt.auth_context()->IsPeerAuthenticated();
    
    lg_.strm(sl::debug) << "is client authenticated? "  << (int) isauth;
    //WARNING: MUST FIX BELOW BEFORE USING!
//     if(!isauth)
//     {
//         foxtrot_server_specific_error("not connected using TLS, refusing to issue challenge",
//                                       repl, respond, lg_, tag, error_types::ft_AuthError);
//         repl.set_statusmsg("not connected using TLS, refusing to issue challenge");
//         return true;
//     }
//     
    
    auto challenge = authhand_->get_challenge_binary(req.userid());
    
    
    
    repl.set_challenge(challenge.first.data(),challenge.first.size());
    repl.set_challengeid(challenge.second);
    
    respond.Finish(repl, grpc::Status::OK, tag);
    return true;
}
