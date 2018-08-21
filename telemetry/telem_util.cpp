#include "telem_util.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "TelemetryServer.h"
#include <string>
#include <iostream>
#include <rttr/variant.h>
#include "Logging.h"
using std::cout;
using std::endl;

using std::string;
using boost::property_tree::ptree;

const int default_ticks = 1;
const string default_subtopic = "";



void configure_telemetry_server(const std::string& fname, foxtrot::Client& cl, foxtrot::TelemetryServer& telemserv)
{
  foxtrot::Logging lg("configure_telemetry_server");
  ptree pt;
  boost::property_tree::json_parser::read_json(fname,pt);
  
  telemserv.set_tick_ms(pt.get<int>("server.tick_ms"));
  telemserv.set_topic(pt.get<string>("server.topic"));
  
  auto sd = cl.DescribeServer();
  
  
  for(auto& dev : pt.get_child("devices"))
  {
      auto devname = dev.first;
      lg.Info("configuring device: " + devname);
      auto devid = foxtrot::find_devid_on_server(sd,devname);
      if(devid < 0)
      {
	lg.Error("couldn't find device on server..>");
	throw std::runtime_error("couldn't find device on server");
      }
      
      for(auto& capability: dev.second)
      {
	auto ticks = capability.second.get<int>("ticks",default_ticks);
	auto telemname = capability.first;
	auto name = capability.second.get<string>("name",capability.first);
	auto subtopic = capability.second.get<string>("subtopic",default_subtopic);
	
	cout << capability.first << endl;
	cout << "---ticks: " << ticks << endl;
	cout << "---name: " << name << endl;
	cout << "---subtopic: " << subtopic << endl;
	  
	
	auto capidx = foxtrot::find_capability(sd,devid,name);
	if(capidx < 0)
	{
	  lg.Error("can't find capability on device");
	  throw std::runtime_error("can't find capability on device");
	}
	
	
	auto args = capability.second.get_child_optional("args");
	auto numargs_required = foxtrot::get_number_of_args(sd,devid,capidx);
	lg.Debug("numargs_required: " + std::to_string(numargs_required));
	
	if(numargs_required && !args.is_initialized())
	{
	  lg.Error("require arguments but got none");
	  lg.Info("args required: " + std::to_string(numargs_required));
	  throw std::runtime_error("require arguments but didn't get any");
	};
	
	if(args.is_initialized() && ( args.get().size() != numargs_required))
	{
	  lg.Error("incorrect number of arguments supplied");
	  throw std::runtime_error("incorrect number of arguments");
	}
	
	std::vector<foxtrot::ft_variant> rttrargs;
	rttrargs.resize(numargs_required);
	
	if(args)
	{
	  cout << "----- have args" << endl; 
	  //TODO: CHECK arg suitability BEFORE adding to server
	  for(auto& arg : *args)
	  {
	      auto arg_position = foxtrot::get_arg_position(sd,devid,capidx,arg.first);
	      if(arg_position <0 )
	      {
		lg.Error("incorrect arg name for function");
		throw std::runtime_error("incorrect arg name");
	      }
	      
	      //auto argtp = static_cast<foxtrot::value_types>(sd.devs_attached().at(devid).caps()[capidx].argtypes()[arg_position]);
	      auto argtp = static_cast<foxtrot::value_types>( *( (sd.devs_attached().at(devid).caps().begin() + capidx)->argtypes().begin() + arg_position ) );
	      
	      switch(argtp)
	      {
		case(foxtrot::value_types::FLOAT_TYPE): 
		  rttrargs[arg_position] = arg.second.get_value<double>(); break;
		case(foxtrot::value_types::INT_TYPE): 
		  rttrargs[arg_position] = arg.second.get_value<int>(); break;
		case(foxtrot::value_types::BOOL_TYPE): 
		  rttrargs[arg_position] = arg.second.get_value<bool>(); break;
		case(foxtrot::value_types::STRING_TYPE):
		  rttrargs[arg_position] = arg.second.get_value<std::string>(); break;
		
		default:
		  throw std::logic_error("argument type not yet implemented for telemetry");
		
	      }
	      
	  }
	  
	    
	}
	
	bool do_bcast = true;
	
	auto  bcast = capability.second.get_optional<bool>("broadcast");
	if(bcast.is_initialized())
	{
	  do_bcast = *bcast;  
	};
	
	  
	  auto fun = [devid,name,rttrargs] (foxtrot::Client& cl) {
		return cl.InvokeCapability(devid,name,rttrargs);
	  };
	
	if(do_bcast)
	{	  
	  telemserv.AddTelemetryItem(fun,ticks,telemname,subtopic);
	}
	else
	{
	  telemserv.AddNonTelemetryItem(fun,ticks,telemname);
	}
	
      }
	
      
      
    
  }
  

}
