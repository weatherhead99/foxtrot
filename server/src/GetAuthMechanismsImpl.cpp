#include "GetAuthMechanismsImpl.h"

namespace foxtrot {
    
    GetAuthMechanismsLogic::GetAuthMechanismsLogic(std::shared_ptr<UserAuthInterface> auth_iface)
    : _auth_iface(auth_iface), _lg("GetAuthMechanismsLogic")
    {
    }
    
    bool GetAuthMechanismsLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond,
                                              HandlerTag* tag)
    {
        if(!_auth_iface)
        {
            foxtrot_server_specific_error("user authentication handler is not installed",
                                          repl, respond, _lg, tag, error_types::ft_AuthError);
            return true;
        }
        
         
        auto mechs  = _auth_iface->get_supported_mechanisms();
        
        _lg.strm(sl::debug) << "adding " << std::to_string(mechs.size()) << "supported mechanisms";
        
        for(auto& mech : mechs)
            repl.add_mechanism_name(mech);
        
        
        respond.Finish(repl, grpc::Status::OK, tag);
        return true;
        
    };
    
}
