#pragma once
#include <array>
#include <tuple>
#include <sstream>
#include <type_traits>
#include <memory>
#include <iomanip>

#include <rttr/type>

#include <foxtrot/Device.h>
#include "archon.h"
#include "archon_module_mapper.hh"

namespace foxtrot
{
    
namespace devices
{


  template<int Upper, int Lower=1>
  struct ArchonChannelBoundsChecker
  {
    inline constexpr void checkChannel(int channel)
    {
      if(channel < Lower or channel > Upper)
	throw std::out_of_range("invalid channel number");
    }

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
      template<detail::ArchonNumeric T>
      void writeConfigKey(const string& key, T& val)
      {
	writeConfigKey(key, std::to_string(val));
      }
            
      
      string readConfigKey(const string& key);
      template<typename Ret>
      Ret readConfigKey(const string& key)
      {
	auto keystr = readConfigKey(key);
	if constexpr(std::is_same_v<Ret, int>)
	  return std::stoi(keystr);
	else if constexpr(std::is_same_v<Ret, double>)
	  return std::stod(keystr);
	else if constexpr(std::is_same_v<Ret, bool>)
	  return std::stoi(keystr);
	else
	  throw std::logic_error("unknown type supplied to readKeyValue");
      }
	    
	    double getTemp();
	    void apply();
	    
	    
	    const archon& getArchon();
	    short unsigned getmodpos();

      template<typename T>
      static std::unique_ptr<T> constructModule(archon& arch, int modpos)
      {
	std::unique_ptr<T> out(new T(arch, modpos));
	return out;
      }
      
	    
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


  template<typename T> concept ArchonModuleType = std::is_base_of_v<ArchonModule, T>;

  template<ArchonModuleType Module>
  std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos)
  {
    return std::make_unique<Module>(arch, modpos);
  }


  std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos, archon_module_types tp);
  
    
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
