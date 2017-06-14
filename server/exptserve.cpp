
//WARNING: some bug appears if this include is moved down the list...
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
    std::string parameterfile;
    int debuglevel;
    po::options_description desc("experiment server for foxtrot devices. Allowed options:");
    desc.add_options()
    ("setupfile,s",po::value<std::string>(&setupfile),"device setup file")
    ("parameterfile,p",po::value<std::string>(&parameterfile),"device setup parameters (JSON file)")
    ("servername,n",po::value<std::string>(&servname),"server name")
    ("dump", po::value<std::string>(), "dump server JSON description")
    ("debuglevel,d",po::value<int>(&debuglevel)->default_value(3),"debugging output level");
    
    po::positional_options_description pdesc;
    pdesc.add("setupfile",1).add("servername",1).add("parameterfile",1);
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(),vm);
    
    po::notify(vm);
    
    if(debuglevel < 0 || debuglevel > 5)
    {
      cout << "invalid debug level specified!" << endl;
      return 1;
    }
    foxtrot::setLogFilterLevel(static_cast<sl>(5 - debuglevel));
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

    std::unique_ptr<std::map<std::string,foxtrot::parameterset>> params = nullptr;
    
    if(!vm.count("parameterfile"))
    {
      cout << "WARNING: no parameter file supplied, continuing without parameters..." << endl;
    }
    else
    {
      
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
