#include "ProtocolUtilities.h"

#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/optional.hpp>

#include <iostream>
#include "Logging.h"

using boost::property_tree::ptree;


struct variant_ptree_wrapper
{
 variant_ptree_wrapper(const ptree& pt) : _pt(pt), _null(true) {};
  
 template<typename T> void operator()(const T&)
 {
   auto res = _pt.get_value_optional<T>();
   if(res.is_initialized())
   {
     _variant = *res;
     _null = false;
   }
   
 }
  
  bool _null ;
  const ptree& _pt;
  foxtrot::parameter _variant;
};



foxtrot::parameter get_variant_from_ptree(const ptree& pt)
{
  auto wrap = variant_ptree_wrapper(pt);
  
  wrap(1);
  wrap(1u);
  wrap(std::string(""));
  
  
  if(wrap._null)
  {
    throw std::out_of_range("couldn't convert ptree value to variant");
  };
  
  return wrap._variant;
  
};




std::map< std::string, foxtrot::parameterset > foxtrot::read_parameter_json_file(const std::string& fname)
{
  foxtrot::Logging lg("read_parameter_json_file");
  
  std::map<std::string,foxtrot::parameterset> out;
  
  ptree pt;
  boost::property_tree::json_parser::read_json(fname,pt);
  
  for(auto& ch : pt)
  {
    std::string parametersetname = ch.first;
    foxtrot::parameterset paramset;

    for(auto& entry : ch.second)
    {
      try
      {
	auto val = get_variant_from_ptree(entry.second);
	paramset.insert({entry.first,val});
      }
      catch(std::out_of_range& err)
      {
	lg.Error("error occurred at value: " + entry.first);
	throw err;
      }
      
    };
    
    out.insert({parametersetname,paramset});
    
    
  };
  
  return out;
}

