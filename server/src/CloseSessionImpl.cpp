#include "CloseSessionImpl.hh"
#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/server/auth_utils.h>

foxtrot::CloseSessionLogic::CloseSessionLogic(std::shared_ptr<SessionManager> sesman)
: _lg("CloseSessionLogic"), _sesman(sesman)
{
    
    
};

bool foxtrot::CloseSessionLogic::HandleRequest(foxtrot::CloseSessionLogic::reqtp& req, foxtrot::CloseSessionLogic::repltp& repl, foxtrot::CloseSessionLogic::respondertp& respond, 
                                               foxtrot::HandlerTag* tag)
{
    _lg.strm(sl::debug) << "processing close session request";
    
    if(req.sessionid().size() == 0)
    {
        foxtrot_server_specific_error("must supply a session secret to close session",
                                      repl, respond, _lg, tag, error_types::ft_ServerError);
        return true;
    }
    
    foxtrot::Sessionid binary_sesid;
    
    if(req.sessionid().size() != binary_sesid.size())
    {

        foxtrot_server_specific_error("invalid session id format",
                                      repl, respond, _lg, tag, error_types::ft_ServerError);
        return true;
    }
    
    std::copy(req.sessionid().begin(), req.sessionid().end(), binary_sesid.begin());
    
    auto sesinfo = _sesman->get_session_info(binary_sesid);
    repl.set_user_identifier(sesinfo.user_identifier);
    repl.set_comment(sesinfo.comment);
    
    auto tt = std::chrono::system_clock::to_time_t(sesinfo.expiry);
    repl.mutable_expiry()->set_seconds(tt);
    
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
