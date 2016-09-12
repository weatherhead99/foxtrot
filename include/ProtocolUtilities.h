#pragma once

#include "CommunicationProtocol.h"
#include "ProtocolError.h"
#include <string>
#include <type_traits>
#include <typeinfo>

namespace foxtrot {
  

template <typename T> void extract_parameter_value(T& param_out, const parameterset& params,
						   const std::string& paramname, bool required=true)
{
  try
  {
    param_out = boost::get<T>(params.at(paramname));
      //TODO: logging here!
  }
  catch(boost::bad_get)
  {
    throw ProtocolError(std::string("invalid type ") + typeid(T).name() +   " specified for parameter: " + paramname );
    
  }
  catch(std::out_of_range)
  {
   if(required)
   {
    throw ProtocolError(std::string("the required parameter ") + paramname + " is undefined"); 
   }
   //TODO: logging here
    
  }
  
  
}

};