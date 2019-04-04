#pragma once

#include "SerialProtocol.h"
#include "ProtocolError.h"
#include <string>
#include <type_traits>
#include <typeinfo>
#include <map>
#include "Logging.h"


namespace foxtrot {
  

template <typename T> bool extract_parameter_value(T& param_out, const parameterset& params,
						   const std::string& paramname, bool required=true)
{
  try
  {
    param_out = boost::get<T>(params.at(paramname));
      //TODO: logging here!
  }
  catch(boost::bad_get)
  {
    foxtrot::Logging lg("extract_parameter_value");
    lg.Error("type held by variant: " + std::to_string(params.at(paramname).which()));
    lg.Error(std::string("type expected: ") + typeid(T).name());
    throw foxtrot::ProtocolError(std::string("invalid type ") + typeid(T).name() +   " specified for parameter: " );
    
    
  }
  catch(std::out_of_range)
  {
   if(required)
   {
    throw std::runtime_error(std::string("the required parameter ") + paramname + " is undefined"); 
   }
   //TODO: logging here
   else
   {
     return false;
   };
    
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

std::map<std::string,foxtrot::parameterset> FOXTROT_EXPORT read_parameter_json_file(const std::string& fname);


};
