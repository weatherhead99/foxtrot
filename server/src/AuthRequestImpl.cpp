#include "AuthRequestImpl.h"


using namespace foxtrot;

AuthRequestLogic::AuthRequestLogic(std::shared_ptr<AuthHandler> authhand)
: authhand_(authhand), lg_("AuthRequestLogic")
{}

bool AuthRequestLogic::HandleRequest(AuthRequestLogic::reqtp& req, AuthRequestLogic::repltp& repl, AuthRequestLogic::respondertp& respond, HandlerTag* tag)
{
    if(!authhand_)
    {
        repl.set_statusmsg("authentication not configured on this server");
        respond.Finish(repl, grpc::Status::OK, tag);
        return true;
    }
    
    
    
    //TODO: refuse if channel does not have TLS enabled!
    grpc::ServerContext servctxt;
    auto isauth = servctxt.auth_context()->IsPeerAuthenticated();
    
    lg_.strm(sl::debug) << "is client authenticated? "  << (int) isauth;
    
    repl.set_challenge(authhand_->get_challenge_string());
    respond.Finish(repl, grpc::Status::OK, tag);
    return true;
}
