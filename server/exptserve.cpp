
#include "ServerImpl.h"
#include "DeviceHarness.h"
#include "dummy/dummyDevice.h"
#include <memory>
#include <iostream>
#include "ExperimentalSetup.h"

using namespace foxtrot;
using std::cout;
using std::endl;

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  std::string setupfile;
    po::options_description desc("experiment server for foxtrot devices. Allowed options:");
    desc.add_options()
    ("setupfile,s",po::value<std::string>(&setupfile),"device setup file");
    
    po::positional_options_description pdesc;
    pdesc.add("setupfile",-1);
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(),vm);
    
    po::notify(vm);
    
    
    if(!vm.count("setupfile"))
    {
     cout << "a setupfile is requried..." << endl; 
      return 1;
    }
    
    DeviceHarness harness;
    foxtrot::ExperimentalSetup setup(setupfile,harness);
    
    
    foxtrot::ServerImpl serv("test server",harness);
    serv.Run();
    


};
