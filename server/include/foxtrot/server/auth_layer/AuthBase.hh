#pragma once
#include <string>
#include <chrono>
#include <array>
#include <vector>
#include <memory>
#include <foxtrot/server/GuardedContainers.hh>
#include <foxtrot/Logging.h>
#include <unordered_map>
#include <map>

using time_point = std::chrono::time_point<std::chrono::system_clock>;
using duration = time_point::duration;
using std::string;
using std::vector;
using std::unique_ptr;

using auth_token = std::array<char, 32>;


namespace foxtrot
{
    struct AuthUser
    {
        string userid;
        string authmech;
        int auth_level;
        int auth_flags;
        time_point expiry;
    };
    
    
    
    class UserProviderInterface
    {
    public:
        virtual AuthUser get_by_userid(const string& userid) = 0;
        
        virtual void add_user(const AuthUser& user) = 0;
        virtual void remove_user(const string& userid) = 0;
        virtual std::vector<AuthUser> get_all_users() = 0;
        
    };
    
    
    class UserAuthProvider 
    {
    public:
        virtual std::vector<string> get_supported_mechanisms() = 0;
        
        
    };
    
    
    using authmethodmap = foxtrot::GuardedMap<std::unordered_map<string, std::weak_ptr<UserAuthProvider>>>;
    using authprovidervec = foxtrot::GuardedSequential<std::vector<std::shared_ptr<UserAuthProvider>>>;
    
    //TODO: use unordered_map here with custom hash??
    using tokenmap = foxtrot::GuardedMap<std::map<auth_token, AuthUser>>;
    
    
    
    class UserAuthInterface
    {
    public:
        UserAuthInterface(unique_ptr<UserProviderInterface> userprovider);
        
        bool verify_call(const auth_token& token, int required_level, int required_flags=0);
        std::vector<string> get_supported_mechanisms();
        void add_provider(std::shared_ptr<UserAuthProvider> provider);
        
    protected:
      
    private:
        authmethodmap _methods;
        authprovidervec _authproviders;
        unique_ptr<UserProviderInterface> _infoprovider;
        tokenmap _tokens;
        Logging _lg;
    };
    
}
