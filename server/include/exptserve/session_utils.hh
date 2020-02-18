#pragma once
#include <foxtrot/server/ServerUtil.h>

namespace foxtrot
{
    class Logging;
    class HandlerTag;
    
    template<typename Req, typename Repl, typename Respond>
    bool require_sessionid_check(const Req& req, Repl& repl, Respond& respond,
                                 Logging& lg, HandlerTag* const tag)
    {
        if(req.sessionid().size() == 0)
        {
            foxtrot_server_specific_error("must supply a session secret to use this call",
                                         repl, respond, lg, tag,
                                         error_types::ft_ServerError);
            return true;
        }
        
        
        if(req.sessionid().size() != std::tuple_size<foxtrot::Sessionid>::value)
        {
            foxtrot_server_specific_error("invalid session id format",
                                          repl, respond, lg, tag,
                                          error_types::ft_ServerError);
            return true;
        };
    return false;
    };
    
    template<typename Repl, typename time_type> 
    void set_expiry_from_time(Repl& repl, const time_type& time)
    {
        auto tt = std::chrono::system_clock::to_time_t(time);
        repl.mutable_expiry()->set_seconds(tt);
    }
    
    template<typename Req>
    std::unique_ptr<time_type> get_expiry(const Req& req)
    {
        auto out = std::unique_ptr<time_type>();
        if(req.has_expiry())
        {
            out = std::make_unique<time_type>(std::chrono::system_clock::from_time_t(req.expiry().seconds()));
            
        }
        
        return out;
    }
    
}
