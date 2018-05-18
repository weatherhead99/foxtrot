#include "ProtocolUtilities.h"

#include "ServerImpl.h"
#include "DeviceHarness.h"
#include "dummy/dummyDevice.h"
#include <memory>
#include <iostream>
#include "ExperimentalSetup.h"
#include <backward.hpp>
#include "Logging.h"
#include <rttr/type>
#include "exptserve.h"
#include <thread>
#include <future>
#include <exception>
#include "config.h"
#include <fstream>

using namespace foxtrot;
using std::cout;
using std::endl;

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    std::string setupfile;
    std::string servname;
    std::string parameterfile;
    
    std::string keyfile;
    std::string crtfile;
    bool forceauth;
    
    auto config_file = foxtrot::get_config_file_path();
    foxtrot::create_config_file(config_file);
    cout << "config file:" << config_file << endl;
    
    int debuglevel =0;
    int nthreads =0 ;
    short unsigned port = 0;
    po::options_description desc("experiment server for foxtrot devices. Allowed options:");
    desc.add_options()
    ("setupfile,s",po::value<std::string>(&setupfile),"device setup file")
    ("parameterfile,p",po::value<std::string>(&parameterfile),"device setup parameters (JSON file)")
    ("servername,n",po::value<std::string>(&servname),"server name")
    ("dump", po::value<std::string>(), "dump server JSON description")
    ("debuglevel,d",po::value<int>(&debuglevel)->default_value(3),"debugging output level")
    ("threads,t",po::value<int>(&nthreads)->default_value(4),"number of server threads to run")
    ("port",po::value<short unsigned>(&port)->default_value(50051),"port to use")
    ("key",po::value<std::string>(&keyfile),"pem for SSL")
    ("crt",po::value<std::string>(&crtfile),"crt for SSL")
    ("forceauth", po::value<bool>(&forceauth)->default_value(true),"force client auth")
    ;
    
    po::positional_options_description pdesc;
    pdesc.add("setupfile",1).add("servername",1).add("parameterfile",1);
    
    po::variables_map vm;
    
    
    po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(),vm);
    
    std::ifstream ifs(config_file);
    if(ifs.good())
    {
        po::store(po::parse_config_file(ifs,desc),vm);
    };
    
    po::notify(vm);


    foxtrot::Logging lg("exptserve");
    
    if(debuglevel < 0 || debuglevel > 5)
    {
      lg.Fatal("invalid debug level specified!");
      return 1;
    }
    foxtrot::setLogFilterLevel(static_cast<sl>(5 - debuglevel));
    foxtrot::setDefaultSink();
    
    if(!vm.count("setupfile"))
    {
     lg.Fatal("a setupfile is requried...");
      return 1;
    }
    
    if(!vm.count("servername"))
    {
      lg.Fatal("server name is required...");
      return 1;
    }

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
    
    
    DeviceHarness harness;
    foxtrot::ExperimentalSetup setup(setupfile,harness,params.get());
    
    if(vm.count("dump"))
    {
      lg.Info("dumping setup...");
      
      auto dumpfile = vm["dump"].as<std::string>();
      dump_setup(harness, dumpfile);
      
      return 0;
    }
    
    
    std::string connstr = "0.0.0.0:" + std::to_string(port);
    
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
      
      serv.SetupSSL(crtfile,keyfile,forceauth);
      
    };
    
    
    
    
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
