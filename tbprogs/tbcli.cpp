#include "client.h"
#include <boost/program_options.hpp>
#include <iostream>
#include "Logging.h"
#include "backward.hpp"
#include "tbcli_heater_logic.h"
#include "tbcli_pressure_logic.h"

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    foxtrot::Logging lg("tbcli");
    namespace po = boost::program_options;
    
    po::options_description desc("Allowed Options");
    
    int debug_level;
    
    desc.add_options()("cmd", po::value<std::string>(), "top level command")
    ("subargs",po::value<std::vector<std::string>>(), "arguments for command")
    ("debug,d",po::value<int>(&debug_level)->default_value(4),"set debug level");
    
    po::positional_options_description pdesc;
    pdesc.add("cmd",1)
    .add("subargs",-1);
    
    po::variables_map vm;
    
    auto parsed = po::command_line_parser(argc,argv)
    .options(desc).positional(pdesc).allow_unregistered().run();
    
    
    po::store(parsed, vm);
    po::notify(vm);
    
    
    foxtrot::setLogFilterLevel(static_cast<sl>(6 - debug_level));
    
    lg.Debug("connecting localhost client..");
    foxtrot::Client client("localhost:50051");
    
    auto servdesc = client.DescribeServer();
    lg.Info("server comment: " + servdesc.servcomment());
    
    
    auto cmd = vm["cmd"].as<std::string>();
    if(cmd == "heater")
    {
        auto devid = find_archon_heater(servdesc);
        if(devid < 0)
        {
            lg.Fatal("no archon heater found on server");
            exit(1);
        }
        
        
        
        lg.Debug("heater command selected");
        
        po::options_description heater_desc("heater options");
        heater_desc.add_options()
        ("subcmd",po::value<std::string>(), "heater subcommand")
        ("value",po::value<std::string>(), "subcommand value");
        
        po::positional_options_description heater_pdesc;
        heater_pdesc.add("subcmd",1)
        .add("value",2);
        
        auto opts = po::collect_unrecognized(parsed.options, po::include_positional);
        opts.erase(opts.begin());
	
        po::store(po::command_line_parser(opts).options(heater_desc).positional(heater_pdesc)
	.run(),vm);
        
        if(!vm.count("subcmd"))
        {
	  lg.Info("printing heater info");
         
	  update_archon_state(servdesc,client);
	  
	  auto output = get_heater_output(client,devid);
	  auto PIDvec = get_heater_coeffs(client,devid);
	  
	  std::cout<<"output:" << output << std::endl;
	  std::cout << "P:" << PIDvec[0] << std::endl;
	  std::cout << "I:" << PIDvec[1] << std::endl;
	  std::cout << "D:" << PIDvec[2] << std::endl;
	  
	  
	  exit(0);
        }
        
        
        auto subcmd = vm["subcmd"].as<std::string>();
        if(subcmd == "enable")
        {
            lg.Info("enabling heater");
            enable_disable_heater_output(client, devid, true);
	    apply_settings(client,devid);
            exit(0);
        }
        else if( subcmd == "disable")
        {
            lg.Info("disabling heater");
            enable_disable_heater_output(client, devid, false);
	    apply_settings(client,devid);
            exit(0);
        }
        else if(subcmd == "target")
        {
            if(!vm.count("value"))
            {
                lg.Info("printing heater target");
                auto tgt = get_heater_target(client,devid);
		std::cout << tgt << std::endl;
                exit(0);
            }
            
            lg.Info("setting heater target");
            
            auto strval = vm["value"].as<std::string>();
            auto val = std::stod(strval);
            
            lg.Debug("target value: " + std::to_string(val));
            set_heater_target(client,devid,val);
	    apply_settings(client,devid);
            exit(0);
                
        }
        else if(subcmd == "ramp")
        {
          if(!vm.count("value"))
          {
              lg.Info("printing ramp value");
              
              exit(0);
          }
          lg.Info("setting ramp value");
          
          auto strval = vm["value"].as<std::string>();
          lg.Debug("strval: " + strval);
          
          if(strval == "enable")
          {
              lg.Info("enabling ramp");
              
              exit(0);
          }
          else if(strval == "disable")
          {
              lg.Info("disabling ramp");
              
              exit(0);
          }
          
          auto val = std::stoul(strval);
          
          lg.Debug("ramp value: " + std::to_string(val));
          
          
          exit(0);
          
        }
        else
        {
         lg.Fatal("invalid heater subcommand");
         exit(1);
            
        }
        
    }
    
    
    else if(cmd == "temp")
    {
	auto devid = find_archon_heater(servdesc);
        if(devid < 0)
        {
            lg.Fatal("no archon heater found on server");
            exit(1);
        }
        
        
        update_archon_state(servdesc,client);
	
	auto temps = get_temperatures(client,devid);
	
	std::cout << "tank:" << temps.first << std::endl;
	std::cout << "stage:" << temps.second << std::endl;
	    
    }
    
    else if(cmd == "pressure")
    {
     auto devid = find_pressure_gauge(servdesc);
     if(devid < 0)
     {
         lg.Fatal("no TPG362 gauge controller found on server");
         exit(1);
     }
     
     auto pres_cryo = get_cryostat_pressure(client,devid);
     auto pres_pump = get_pump_pressure(client,devid);
     
     std::cout << "cryostat:" << pres_cryo   << std::endl;
     std::cout << "pump:" << pres_pump << std::endl;
        
        
    }
    
    else if(cmd == "powermeter")
    {
      auto devid = foxtrot::find_devid_on_server(servdesc, "newport2936R");
      if(devid < 0)
      {
	lg.Fatal("no newport 2936R found on server");
	exit(1);
      }
      
      auto power = boost::get<double>(client.InvokeCapability(devid,"getPower",{}));
      auto units = boost::get<std::string>(client.InvokeCapability(devid,"getUnits",{})); 
      std::cout << "units length: " << units.size() << std::endl;
      std::cout << "power: " << power <<  " " << units << std::endl;
    }

    else if(cmd == "gauge") 
      {

	auto devid = find_pressure_gauge(servdesc);
	if(devid < 0)
	  {
	    lg.Fatal("no TPG362 gauge controller found on server");
	    exit(1);
	  }
     

	po::options_description gauge_desc("gauge options");
        gauge_desc.add_options()
        ("subcmd",po::value<std::string>(), "gauge subcommand")
        ("value",po::value<std::string>(), "subcommand value");
        
        po::positional_options_description gauge_pdesc;
        gauge_pdesc.add("subcmd",1)
        .add("value",2);
	
	auto opts = po::collect_unrecognized(parsed.options, po::include_positional);
        opts.erase(opts.begin());
	
        po::store(po::command_line_parser(opts).options(gauge_desc).positional(gauge_pdesc)
	.run(),vm);
    
	
	if(!vm.count("subcmd"))
	  {
	    std::cout << "require a subcmd!" << std::endl;
	    exit(1);

	  }

	auto subcmd = vm["subcmd"].as<std::string>();
	if(subcmd == "on")
	  {
	    lg.Info("turning gauges on");
	    gauges_onoff(client,devid, true);
	  }
	else if(subcmd == "off")
	  {
	    lg.Info("turning gauges off");
	    gauges_onoff(client,devid,false);
	  }

	


      }



    else
      {
	std::cout << "unregognised command: " << cmd << std::endl;
      }



}
