#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include <memory>
#include <shared_mutex>
#include <algorithm>

using std::string;

namespace foxtrot
{
    
    using ft_std_variant = std::variant<double, int, bool, string>;
    
    class FlagMap : public std::enable_shared_from_this<FlagMap> 
    {
    public:
        void set(const string& flagname, const ft_std_variant& val);
        ft_std_variant get(const string& flagname) const;
        void drop(const string& flagname);
        
        template<typename F> 
        void for_each_readonly(F apply)
        {
            std::shared_lock lck(_mut);
            std::for_each(_vars.cbegin(), _vars.cend(), apply);
        };
        
        template<typename F>
        void for_each_readwrite(F apply)
        {
            std::lock_guard lck(_mut);
            std::for_each(_vars.begin(), _vars.end(), apply);
        };
        
        
    private:
        std::unordered_map<string, ft_std_variant> _vars;
        std::shared_mutex _mut;
    };
    
    
}
