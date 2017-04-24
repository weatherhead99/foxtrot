
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

using namespace foxtrot;
using std::cout;
using std::endl;

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    std::string setupfile;
    std::string servname;
    po::options_description desc("experiment server for foxtrot devices. Allowed options:");
    desc.add_options()
    ("setupfile,s",po::value<std::string>(&setupfile),"device setup file")
    ("servername,n",po::value<std::string>(&servname),"server name")
    ("dump", po::value<std::string>(), "dump server JSON description");
    
    po::positional_options_description pdesc;
    pdesc.add("setupfile",1).add("servername",1);
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(),vm);
    
    po::notify(vm);
    foxtrot::setDefaultSink();
    
    if(!vm.count("setupfile"))
    {
     cout << "a setupfile is requried..." << endl; 
      return 1;
    }
    
    if(!vm.count("servername"))
    {
      cout << "server name is required..." << endl;
      return 1;
    }
    
    
    DeviceHarness harness;
    foxtrot::ExperimentalSetup setup(setupfile,harness);
    
    if(vm.count("dump"))
    {
      cout << "dumping setup..." << endl;
      
      auto dumpfile = vm["dump"].as<std::string>();
      dump_setup(harness, dumpfile);
      
      return 0;
    }
    
    
    foxtrot::ServerImpl serv(servname,harness);
    serv.Run();
    
};
