#include <foxtrot/server/FlagMap.hh>
#include <stdexcept>

using std::string;

void foxtrot::FlagMap::set(const string& flagname, const foxtrot::ft_std_variant& val)
{
    std::lock_guard lck(_mut);
    _vars[flagname] = val;
};

void foxtrot::FlagMap::drop(const std::string& flagname)
{
    std::lock_guard lck(_mut);
    
    auto it = _vars.find(flagname);
    if(it == _vars.end())
        throw std::out_of_range("no such flag");
    
    _vars.erase(it);
}

foxtrot::ft_std_variant foxtrot::FlagMap::get(const string& flagname) const
{
    auto it = _vars.find(flagname);
    if(it == _vars.end())
        throw std::out_of_range("invalid server flag");
    
    return _vars.at(flagname);
}

