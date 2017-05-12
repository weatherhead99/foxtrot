#include "TelemetryServer.h"
#include "telem_util.h"
#include <boost/program_options.hpp>
#include <backward.hpp>
#include <iostream>
#include <sstream>
#include "client.h"
#include "Logging.h"
using std::cout;
using std::cerr;
using std::endl;


namespace po = boost::program_options;

int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  
  po::options_description desc("telemetry broadcaster for foxtrot telemetry.");
  foxtrot::setDefaultSink();
  
  std::string configfile;
  int port;
  std::string addr;
  int debuglevel;
  
  desc.add_options()
  ("configfile,c",po::value<std::string>(&configfile),"config file")
  ("port,p",po::value<int>(&port)->default_value(50051),"port to connect to")
  ("addr,a", po::value<std::string>(&addr)->default_value("0.0.0.0"),"address of server")
  ("debuglevel,d", po::value<int>(&debuglevel)->default_value(3),"debugging output level");

  po::positional_options_description pdesc;
  pdesc.add("configfile",-1);
  
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(), vm);
  
  po::notify(vm);
  
  if(!vm.count("configfile"))
  {
    cerr << "a configfile is required..." << endl;
    return -1;
  }
  
  
  std::ostringstream oss;
  oss << addr << ":" << port;

  foxtrot::Client cl(oss.str());
  foxtrot::TelemetryServer telemserv("",cl,2000);
  
  configure_telemetry_server(configfile,cl,telemserv);


}
