#include "CloseSessionImpl.hh"
#include <foxtrot/server/ServerUtil.h>
#include "session_utils.hh"

foxtrot::CloseSessionLogic::CloseSessionLogic(std::shared_ptr<SessionManager> sesman)
: _lg("CloseSessionLogic"), _sesman(sesman)
{
    
    
};

bool foxtrot::CloseSessionLogic::HandleRequest(foxtrot::CloseSessionLogic::reqtp& req, foxtrot::CloseSessionLogic::repltp& repl, foxtrot::CloseSessionLogic::respondertp& respond, 
                                               foxtrot::HandlerTag* tag)
{
    _lg.strm(sl::debug) << "processing close session request";
    
    if(require_sessionid_check(req, repl, respond, _lg, tag))
        return true;
    
    foxtrot::Sessionid binary_sesid;
    std::copy(req.sessionid().begin(), req.sessionid().end(), binary_sesid.begin());
    
    auto sesinfo = _sesman->get_session_info(binary_sesid);
    repl.set_user_identifier(sesinfo.user_identifier);
    repl.set_comment(sesinfo.comment);
    
    set_expiry_from_time(repl, sesinfo.expiry);
    
    unsigned const_zero = 0;
    repl.mutable_devices()->Resize(sesinfo.devices.size(), const_zero);
    std::copy(sesinfo.devices.begin(), sesinfo.devices.end(), repl.mutable_devices()->begin());
    
    std::string const_empty = "";
    for(auto& flag : sesinfo.flags)
    {
        repl.add_flags(flag);
    }
    
    bool success = _sesman->close_session(binary_sesid);
    if(!success)
    {
        foxtrot_server_specific_error("unable to close session, perhaps secret is incorrect", repl, respond, _lg, tag, error_types::ft_ServerError);
        return true;
    }
    
    respond.Finish(repl, grpc::Status::OK, tag);
    return true;
    
}
