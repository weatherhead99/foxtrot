#pragma once
#include <array>
#include <tuple>

#include "Device.h"
#include "archon.h"
#include <sstream>
#include <type_traits>

namespace foxtrot
{
    
namespace devices
{
    
    enum class archon_module_types : short unsigned
    {
        None  = 0,
        Driver = 1,
        AD = 2,
        LVBias = 3,
        HVBias = 4,
        Heater = 5,
        HeaterX  = 6,
        HS = 7,
        HVXBias = 8,
        LVXBias = 9,
        LVDS = 10
    };
    
    
    class ArchonModule : public Device
    {
    public:
            const std::array<char,16>& getID() const;
            const std::array<char,3>& getVersion() const;
            short unsigned getRev() const;
            
    protected:
            short unsigned _modpos;
            archon& _arch;
            ArchonModule(archon& arch, short unsigned modpos);
	    
    private:
            
            //WARNING: is the lifetime of this guaranteed?
            
            std::array<char,16> _id;
            std::array<char,3> _version;
            short unsigned _rev;
            
            archon_module_types _modtype;
	    
	    virtual void update_variables() = 0;
	    
            
    };
    
    
    string get_module_variable_string(int modpos, const string& name, const ssmap& map, char delim='/');
    
    template <typename T> typename std::enable_if<std::is_integral<T>::value,T>::type  extract_module_variable(
      int modpos, const string& name,const ssmap& map, char delim='/')
    {
      auto stval = get_module_variable_string(modpos,name,map,delim);
      return std::stoi(stval);
    }
    
    template <typename T> typename std::enable_if<std::is_floating_point<T>::value,T>::type extract_module_variable(
      int modpos, const string& name,const ssmap& map, char delim='/')
    {
      auto stval = get_module_variable_string(modpos,name,map,delim);
      return std::stof(stval);
    }
    
    
    
    
    
};
    
};
