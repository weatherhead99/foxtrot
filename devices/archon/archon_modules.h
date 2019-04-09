#pragma once
#include <array>
#include <tuple>
#include <sstream>
#include <type_traits>
#include <memory>
#include <iomanip>

#include <rttr/type>

#include <foxtrot/server/Device.h>
#include "archon.h"

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
      friend class foxtrot::devices::archon;
      RTTR_ENABLE(Device);
    public:
      
            const string& getID() const;
            const std::array<char,3>& getVersion() const;
            short unsigned getRev() const;
	    virtual const std::string getTypeName() const = 0;
            
	    void writeConfigKey(const string& key, const string& val);
	    string readConfigKey(const string& key);
	    
	    double getTemp();
	    
	    
	    void apply();
	    
	    
	    const archon& getArchon();
	    short unsigned getmodpos();
	    
	    
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
      auto stval = get_module_variable_string(modpos,name,map,delim);
      return std::stoi(stval);
    }
    
    template <typename T> typename std::enable_if<std::is_floating_point<T>::value,T>::type extract_module_variable(
      int modpos, const string& name,const ssmap& map, char delim='/')
    {
      auto stval = get_module_variable_string(modpos,name,map,delim);
      return std::stof(stval);
    }
    
    
    template <typename T> void archon_hex_stream_configure(T& stream, int width=2)
    {
      stream << std::setw(width) << std::setfill('0') << std::hex << std::uppercase ;
    }
    
    
};
    
};
