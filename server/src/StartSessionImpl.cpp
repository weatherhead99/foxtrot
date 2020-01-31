#include "StartSessionImpl.hh"
#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/server/auth_utils.h>

foxtrot::StartSessionLogic::StartSessionLogic(
    std::shared_ptr<foxtrot::SessionManager> sesman)
: BaseSessionLogicImpl("StartSessionLogic", sesman)
{};

void populate_reply(foxtrot::session_info& repl, const foxtrot::ft_session_info& in)
{
}


bool foxtrot::StartSessionLogic::HandleRequest(foxtrot::StartSessionLogic::reqtp& req,
                                               foxtrot::StartSessionLogic::repltp& repl,
                                               foxtrot::StartSessionLogic::respondertp& respond,
                                               foxtrot::HandlerTag* tag)
{
    _lg.strm(sl::debug) << "processing start session request";
    
    if(req.user_identifier().size() == 0)
    {
        foxtrot_server_specific_error("must supply a user identifier to start session", repl,
                                      respond, _lg, tag, error_types::ft_ServerError);
        return true;
    }
    
        std::vector<unsigned> vecdev(req.devices().begin(), req.devices().end());
        std::vector<std::string> vecflag(req.flags().begin(), req.flags().end());
        
        
        time_type* requested_expiry = nullptr;
        if(req.has_expiry())
            *requested_expiry = std::chrono::system_clock::from_time_t(req.expiry().seconds());
    
        try
        {
        auto [sessionid, internalid] = _sesman->start_session(req.user_identifier(), req.comment(), &vecdev, 
                               &vecflag, requested_expiry);
        
        std::string secstring(sessionid.begin(), sessionid.end());
        
        repl.set_sessionid(secstring);
        repl.set_user_identifier(req.user_identifier());
        repl.set_comment(req.comment());
        
        repl.mutable_devices()->CopyFrom(req.devices());
        repl.mutable_flags()->CopyFrom(req.flags());
        
        auto expiry = _sesman->get_session_info(internalid).expiry;
        
        //WARNING: probably doesn't work on Windows!!
        auto tt = std::chrono::system_clock::to_time_t(expiry);
        repl.mutable_expiry()->set_seconds(tt);
        
        respond.Finish(repl, grpc::Status::OK, tag); 
        }
        catch(short unsigned& i)
        {
            auto owner = _sesman->get_session_info(i);
            repl.set_user_identifier(owner.user_identifier);
            repl.set_comment(owner.comment);
            
            auto tt_owner = std::chrono::system_clock::to_time_t(owner.expiry);
            repl.mutable_expiry()->set_seconds(tt_owner);
            
            foxtrot_server_specific_error("Can't start session as it conflicts with another session",
                repl, respond, _lg, tag, error_types::ft_ServerError);
        }
        catch(std::out_of_range& err)
        {
            if(requested_expiry)
            {
                auto seslen = _sesman->get_session_length();
                auto now = std::chrono::system_clock::now();
                auto max_expiry = now + seslen;
                if(*requested_expiry > max_expiry)
                {
                    auto tt = std::chrono::system_clock::to_time_t(max_expiry);
                    repl.mutable_expiry()->set_seconds(tt);
                    foxtrot_server_specific_error("requested an expiry beyond the maximum allowed",
                        repl, respond, _lg, tag, error_types::ft_ServerError);
                    
                    return true;
                }
            }
            throw err;
        }
        
        return true;
}
