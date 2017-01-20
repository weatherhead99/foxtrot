#pragma once
#include <array>
#include <tuple>

#include "Device.h"
#include "archon.h"
#include <sstream>
#include <type_traits>
#include <memory>

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
	//WRONG!
//         HeaterX  = 6,
        HS = 7,
        HVXBias = 8,
        LVXBias = 9,
        LVDS = 10,
	HeaterX = 11,
	XVBias = 12
    };
    
    
    class ArchonModule : public Device
    {
    public:
            const string& getID() const;
            const std::array<char,3>& getVersion() const;
            short unsigned getRev() const;
	    virtual const std::string getTypeName() const = 0;
            
            
    protected:
            short unsigned _modpos;
            archon& _arch;
            ArchonModule(archon& arch, short unsigned modpos);
	    
    private:
            
            //WARNING: is the lifetime of this guaranteed?
            
            string _id;
            std::array<char,3> _version;
            short unsigned _rev;
            
            archon_module_types _modtype;
	    
	    virtual void update_variables() = 0 ;
	    
            
    };
    
    
    string get_module_variable_string(int modpos, const string& name, const ssmap& map, char delim='/');
    
    template <typename T> typename std::enable_if<std::is_integral<T>::value,T>::type  extract_module_variable(
      int modpos, const string& name,const ssmap& map, char delim='/')
    {
//       std::cout << "getting variable: " << name << std::endl;
      auto stval = get_module_variable_string(modpos,name,map,delim);
//       std::cout << "stval: " << stval << std::endl;
      return std::stoi(stval);
    }
    
    template <typename T> typename std::enable_if<std::is_floating_point<T>::value,T>::type extract_module_variable(
      int modpos, const string& name,const ssmap& map, char delim='/')
    {
//       std::cout << "getting variable: " << name << std::endl;
      auto stval = get_module_variable_string(modpos,name,map,delim);
//       std::cout << "stval: " << stval << std::endl;
      
//       std::cout << "trying stoi" << std::stoi(stval) << std::endl;
//       std::cout << "trying stof" << std::stof(stval) << std::endl;
      
      return std::stof(stval);
    }
    
    
    
    
    
};
    
};
