#include <foxtrot/server/auth_layer/AuthBase.hh>

namespace foxtrot {
    
    UserAuthInterface::UserAuthInterface(unique_ptr<UserProviderInterface> userprovider)
    : _infoprovider(std::move(userprovider)), _lg("UserAuthInterface")
    {
    };
    
    void UserAuthInterface::add_provider(std::shared_ptr<UserAuthProvider> provider)
    {
        auto provider_mechs = provider->get_supported_mechanisms();
        
        for(auto& provider_mech : provider_mechs)
        {
            if(_methods.in(provider_mech))
                throw std::runtime_error( std::string("tried to duplicate mechanism: ") + provider_mech);
                
        }
        
        _lg.strm(sl::debug) << "adding provider to supported list...";
        _authproviders.apply_operation_readwrite([&provider] (auto& vec) { vec.push_back(provider);});
        _lg.strm(sl::debug) << "updating supported mechanism map...";
        
        for(auto& provider_mech : provider_mechs)
        {
            _methods.set(provider_mech, provider);
        }
        
    };
    
    
    std::vector<string> UserAuthInterface::get_supported_mechanisms()
    {
        std::vector<string> out;
        _methods.for_each_readonly([&out] (auto& item) { out.push_back(item.first);});
        return out;
    }
    
}
