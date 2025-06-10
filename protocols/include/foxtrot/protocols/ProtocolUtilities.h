#pragma once
#include <string>
#include <type_traits>
#include <typeinfo>
#include <map>
#include <optional>

#include <foxtrot/Logging.h>
#include <foxtrot/ProtocolError.h>

#include <foxtrot/protocols/SerialProtocol.h>


template<typename V>
std::string get_variant_held_typename(V&& var)
{

  std::string tpname;
  std::visit([&tpname] (auto&& arg) { tpname = typeid(arg).name();}, var);
  return tpname;
}



namespace foxtrot {

  namespace detail
  {
    template<typename T>
    constexpr bool is_optional(const T&) { return false;};

    template<typename T>
    constexpr bool is_optional(const std::optional<T>&){return true;};
      

  }

template <typename T> bool extract_parameter_value(T& param_out, const parameterset& params,
						   const std::string& paramname, bool required=true)
{

  foxtrot::Logging lg("extract_parameter_value");
  try
  {
    if constexpr(detail::is_optional(param_out))
      {
	if(required)
	  lg.strm(sl::warning) << "passed required=True but the output type is a std::optional. Likely a bug";
	*param_out = std::get<typename T::value_type>(params.at(paramname));
      }
    else
      {
	param_out = std::get<T>(params.at(paramname));
      }
      //TODO: logging here!
  }
  catch(std::bad_variant_access)
  {
    auto type_held = get_variant_held_typename(params.at(paramname));
    lg.Error("type held by variant: " + type_held);
    lg.Error(std::string("type expected: ") + typeid(T).name());
    throw foxtrot::ProtocolError(std::string("invalid type ") + typeid(T).name() +   " specified for parameter: " );

  }
  catch(std::out_of_range)
  {
   if(required)
    throw std::runtime_error(std::string("the required parameter ") + paramname + " is undefined"); 
   //TODO: logging here
   else
     return false; 
  }
  return true;
  
};



template <typename keytp, typename valtp> 
void extract_parameter_map_cast(const std::map<keytp,valtp>& map, valtp& param_out, const parameterset& params,
  const std::string& paramname, bool required=true)
{
  
  //extract parameter to a key value
  keytp temp_param;
  auto did_something = extract_parameter_value(temp_param,params,paramname,required);
  
  try
  {
    //only try and overwrite parameter if a value was actually supplied via the parameterset
    if(did_something)
    {
      param_out = map.at(temp_param);
    };
  }
  catch(std::out_of_range)
  {
    throw std::runtime_error(std::string("invalid value for parameter: " ) + paramname);
  }
  

};


//std::string read_until_endl(SerialProtocol* proto, unsigned readlen, char endlchar='\n');

std::map<std::string,foxtrot::parameterset>  read_parameter_json_file(const std::string& fname);


};
