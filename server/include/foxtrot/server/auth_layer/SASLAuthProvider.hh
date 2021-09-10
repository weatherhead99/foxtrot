#pragma once

#include "AuthBase.hh"
#include <foxtrot/Logging.h>

struct Gsasl;

namespace foxtrot
{
    
    class SASLAuthProvider : public UserAuthProvider
    {
    public:
        SASLAuthProvider();
        ~SASLAuthProvider();
        std::vector<string> get_supported_mechanisms() override;
        
        
        
    private:
        Gsasl* ctx = nullptr;
        Logging lg;
        
    };
    
    
    
    
}
