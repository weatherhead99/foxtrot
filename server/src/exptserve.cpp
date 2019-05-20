#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <exception>
#include <fstream>

#include <rttr/type>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include <boost/filesystem.hpp>

#include <foxtrot/backward.hpp>
#include <foxtrot/Logging.h>
#include <foxtrot/config.h>

#include <foxtrot/protocols/ProtocolUtilities.h>
#include <foxtrot/server/DeviceHarness.h>
#include <foxtrot/server/ExperimentalSetup.h>

#include "exptserve.h"
#include "ServerImpl.h"

using namespace foxtrot;
using std::cout;
using std::endl;

namespace po = boost::program_options;

int main(int argc, char** argv)
{
#ifndef linux
    std::set_terminate([] () 
    {
       auto eptr = std::current_exception();
       try{
        if(eptr)
        {
            std::rethrow_exception(eptr);
        }
       }
       catch(std::exception& err)
       {
           cout << "caught exception, message: " << endl;
           cout  << err.what() << endl;
           cout << "now aborting.." << endl;
           
       };
        
       std::abort();
    });
        
#endif
    
    
    
    backward::SignalHandling sh;
    std::string setupfile;
    std::string servname;
    std::string parameterfile;
    
    std::string keyfile;
    std::string crtfile;
    bool forceauth;
    
    std::string bindstr;
    auto servercreds = foxtrot::get_config_file_path("FOXTROT_CREDS", "server_creds.json");
    int cred_validity_hours;
    
    foxtrot::Logging lg("exptserve");
    
    auto config_file = foxtrot::get_config_file_path("FOXTROT_CONFIG", "exptserve.config");
    foxtrot::create_config_file(config_file);
    lg.strm(sl::info) <<  "config file:" << config_file;
    
    int debuglevel =0;
    int nthreads =0 ;
    short unsigned port = 0;
    po::options_description desc("experiment server for foxtrot devices. Allowed options:");
    desc.add_options()
    ("setupfile,s",po::value<std::string>(&setupfile)->required(),"device setup file")
    ("parameterfile,p",po::value<std::string>(&parameterfile),"device setup parameters (JSON file)")
    ("servername,n",po::value<std::string>(&servname)->required(),"server name")
    ("dump", po::value<std::string>(), "dump server JSON description")
    ("debuglevel,d",po::value<int>(&debuglevel)->default_value(3),"debugging output level")
    ("threads,t",po::value<int>(&nthreads)->default_value(4),"number of server threads to run")
    ("port",po::value<short unsigned>(&port)->default_value(50051),"port to use")
    ("key",po::value<std::string>(&keyfile),"pem for SSL")
    ("crt",po::value<std::string>(&crtfile),"crt for SSL")
    ("forceauth", po::bool_switch(&forceauth),"force client auth")
    ("bindstr", po::value<std::string>(&bindstr)->default_value("0.0.0.0"), "socket listen string")
    ("pushbullet_api_key", po::value<std::string>(),"API key for pushbullet notifications")
    ("pushbullet_default_title", po::value<std::string>()->default_value(""),
     "default title for pushbullet notifications")
    ("pushbullet_default_channel", po::value<std::string>()->default_value(""), 
     "default channel for pushbullet notifications")
    ("servercreds,S", po::value<std::string>(&servercreds), "authentication credentials (JSON) file")
    ("credvalid,v", po::value<int>(&cred_validity_hours)->default_value(24), "length of credential validity (hours)")
    ("help","display usage information");
    
    po::positional_options_description pdesc;
    pdesc.add("setupfile",1).add("servername",1).add("parameterfile",1);
    
    po::variables_map vm;
    
    foxtrot::load_config_file(config_file,desc,vm);
    po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(),vm);

    
 if(vm.count("help"))
    {
        std::cout << "Usage:" << argv[0] << " setupfile servername parameterfile [options]" << std::endl;
        std::cout << "description of options: " << std::endl;
        std::cout << desc << std::endl;
        std::exit(0);
    }
    
    try{
        po::notify(vm);
    }
    catch(boost::program_options::required_option& err)
    {
        lg.strm(sl::fatal) << "required option missing...";
        lg.strm(sl::fatal) << err.what();
        std::exit(1);
        
    }
    foxtrot::check_debug_level_and_exit(debuglevel,lg);
    
    foxtrot::setDefaultSink();
    
    
    
    std::unique_ptr<std::map<std::string,foxtrot::parameterset>> params;
    
    if(!vm.count("parameterfile"))
    {
      lg.Warning("no parameter file supplied, continuing without parameters...");
      params.reset(nullptr);
    }
    else
    {
      lg.Info("reading parameter sets...");
      params.reset( new std::map<std::string,foxtrot::parameterset>(foxtrot::read_parameter_json_file(parameterfile)));
      
    }
    
    lg.strm(sl::debug) << "passed all the option tests..";
    
    
    DeviceHarness harness;
    foxtrot::ExperimentalSetup setup(setupfile,harness,params.get());
    
    if(vm.count("dump"))
    {
      lg.Info("dumping setup...");
      
      auto dumpfile = vm["dump"].as<std::string>();
      dump_setup(harness, dumpfile);
      
      return 0;
    }
    
    
    std::string connstr = bindstr + ":" + std::to_string(port);
    
    foxtrot::ServerImpl serv(servname,harness,connstr);
    
    
    
    
    if(vm.count("key"))
    {
      if(!vm.count("crt"))
      {
        lg.Fatal("asked for SSL but only provided key file");
        return -1;
      }
      
      if(!forceauth)
          lg.Warning("client authentication not forced!");
      
      lg.Info("setting up SSL secure channel");
      serv.SetupSSL(crtfile,keyfile,forceauth);
      
    };
    
    
    if(vm.count("pushbullet_api_key"))
    {
        lg.Info("setting up pushbullet notifications...");
        serv.setup_notifications(vm["pushbullet_api_key"].as<std::string>(),
                                 vm["pushbullet_default_title"].as<std::string>(),
                                 vm["pushbullet_default_channel"].as<std::string>());
    }
    
    
    lg.Info("trying to load credentials for authentication");
    if(!boost::filesystem::exists(servercreds))
    {
        lg.Info("no credentials info found, not setting up authentication");
    }
    else
    {
        lg.Info("setting up authentication system");
        serv.setup_auth(servercreds, cred_validity_hours);
    }
    
    if(nthreads > 1)
    {
      lg.Info("running in multithreaded mode...");
      auto excepts = serv.RunMultithread(nthreads);
      auto exitthread = serv.join_multithreaded();
      auto except_ptr = excepts[exitthread].get();
    
      if(except_ptr)
      {
        std::rethrow_exception(except_ptr);
      }
    
      lg.Info("server exited without error...");
    
    }
    else
    {
      lg.Info("running in single threaded mode...");
      serv.Run();
    }
    
};
