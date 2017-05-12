#include "telem_util.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "TelemetryServer.h"
#include <string>
#include <iostream>
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
	auto name = capability.second.get<string>("name",capability.first);
	auto subtopic = capability.second.get<string>("subtopic",default_subtopic);
	
	cout << capability.first << endl;
	cout << "---ticks: " << ticks << endl;
	cout << "---name: " << name << endl;
	cout << "---subtopic: " << subtopic << endl;
	
	auto fun = [devid,name] (foxtrot::Client& cl) {
	      return cl.InvokeCapability(devid,name);
	};
	  
	auto args = capability.second.get_child_optional("args");      
	if(args)
	{
	  cout << "----- have args" << endl; 
	  //TODO: CHECK arg suitability BEFORE adding to server
	  for(auto& arg : *args)
	  {
	    
	    
	  }
	  
	}
	
	
	
	telemserv.AddTelemetryItem(fun,ticks,name,subtopic);
	
      }
	
      
      
    
  }
  

}
