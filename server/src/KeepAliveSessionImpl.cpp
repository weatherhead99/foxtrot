#include "KeepAliveSessionImpl.hh"
#include <foxtrot/server/ServerUtil.h>
#include "session_utils.hh"

using namespace foxtrot;

KeepAliveSessionLogic::KeepAliveSessionLogic(std::shared_ptr<SessionManager> sesman)
: _sesman(sesman), _lg("KeepAliveSessionLogic")
{};

bool KeepAliveSessionLogic::HandleRequest(reqtp & req, repltp& repl, 
                                          respondertp& respond, HandlerTag* tag)
{
    
    _lg.strm(sl::debug) << "processing keep alive session request";
    
    if(require_sessionid_check(req, repl, respond, _lg, tag))
        return true;
    
    foxtrot::Sessionid binary_sesid;
    std::copy(req.sessionid().begin(), req.sessionid().end(), binary_sesid.begin());
    
    _lg.strm(sl::trace) << "getting requested expiry";
    auto requested_expiry = get_expiry(req);
    
    if(requested_expiry == nullptr)
        _lg.strm(sl::trace) << "no requested expiry";
    
    bool success = _sesman->renew_session(binary_sesid, requested_expiry.get());
    
    if(!success)
    {
        foxtrot_server_specific_error("unable to renew session", repl, respond, _lg,
                                      tag, error_types::ft_ServerError);
        return true;
    }
    
    auto sesinfo = _sesman->get_session_info(binary_sesid);
    repl.set_user_identifier(sesinfo.user_identifier);
    repl.set_comment(sesinfo.comment);
    set_expiry_from_time(repl, sesinfo.expiry);
    
    respond.Finish(repl, grpc::Status::OK, tag);
    
    return true;
};
