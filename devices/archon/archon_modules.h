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
#include "archon_module_generic_bias.h"
#include "archon_module_mapper.hh"
#include "../device_utils/stringconv_utils.hh"

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
      friend class foxtrot::devices::ArchonGenericBias;
      RTTR_ENABLE(Device);
    public:
      	void apply();
      
      virtual const std::string getTypeName() const = 0;
      void writeConfigKey(const string& key, const string& val);
      template<detail::ArchonNumeric T>
      void writeConfigKey(const string& key, T& val)
      {
	writeConfigKey(key, std::to_string(val));
      }

      virtual void status(archon_module_status& out, const ssmap& statusmap) const;
      archon_module_status status(const ssmap& statusmap) const;
      archon_module_status status() const;
      const archon_module_info& info() const;

      std::optional<string> readConfigKeyOpt(const string& key) const;

      template<typename Ret, int Base=10>
      std::optional<Ret> readConfigKeyOpt(const string& key) const
      {
	auto valstr = readConfigKeyOpt(key);
	if(valstr.has_value())
	  return number_from_string<Ret, Base>(valstr);
	
      }
      
      string readConfigKey(const string& key) const;
      template<typename Ret, int Base=10>
      Ret readConfigKey(const string& key) const
      {
	auto valstr = readConfigKey(key);
	return number_from_string<Ret, Base>(valstr);
      }
	    


      template<typename T>
      static std::unique_ptr<T> constructModule(std::weak_ptr<archon>& arch, const archon_module_info& inf)
      {
	std::unique_ptr<T> out(new T(arch, inf));
	return out;
      }
      
	    
    protected:
      archon_module_info _info;
      std::weak_ptr<archon> _arch;
      ArchonModule(std::weak_ptr<archon>& arch, const archon_module_info& info);
    protected:
            std::array<char,3> _version;
    };




  template<typename T> concept ArchonModuleType = std::is_base_of_v<ArchonModule, T>;

  template<ArchonModuleType Module>
  std::unique_ptr<ArchonModule> constructModule(std::weak_ptr<archon>& arch, const archon_module_info& info)
  {
    return std::make_unique<Module>(arch, info);
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
