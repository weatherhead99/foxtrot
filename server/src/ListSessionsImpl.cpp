#include "ListSessionsImpl.hh"

using namespace foxtrot;

ListSessionsLogic::ListSessionsLogic(std::shared_ptr<SessionManager> sesman)
: _sesman(sesman), _lg("ListSessionsLogic")
{};

bool ListSessionsLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                                      HandlerTag* tag)
{
    _lg.strm(sl::debug) << "processing list sessions request";
    
    
    _sesman->for_each_readonly([&repl] (const auto& item)
    {
        auto* session_out = repl.add_sessions();
        session_out->set_user_identifier(item.second.user_identifier);
        session_out->set_comment(item.second.comment);
        
        for(auto& devid : item.second.devices)
        {
            session_out->add_devices(devid);
        }
        
        for(auto& flagname : item.second.flags)
        {
            session_out->add_flags(flagname);
        }
        
        auto tt = std::chrono::system_clock::to_time_t(item.second.expiry);
        session_out->mutable_expiry()->set_seconds(tt);
        
    });
    
    respond.Finish(repl, grpc::Status::OK, tag);
    
    return true;
}
