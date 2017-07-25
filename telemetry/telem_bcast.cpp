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
  std::string bindstr;
  
  desc.add_options()
  ("configfile,c",po::value<std::string>(&configfile),"config file")
  ("port,p",po::value<int>(&port)->default_value(50051),"port to connect to")
  ("addr,a", po::value<std::string>(&addr)->default_value("0.0.0.0"),"address of foxtrot server")
  ("debuglevel,d", po::value<int>(&debuglevel)->default_value(3),"debugging output level")
  ("bindstr,b",po::value<std::string>(&bindstr)->default_value("tcp://*:50052"),"bind string for server");

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
  
  if(debuglevel < 0 || debuglevel > 5)
  {
    cerr << "invalid debug level specified!" << endl;
    return -1;
  }
  
  foxtrot::setLogFilterLevel(static_cast<sl>(5 - debuglevel));
  foxtrot::setDefaultSink();
  
  std::ostringstream oss;
  oss << addr << ":" << port;

  foxtrot::Client cl(oss.str());
  foxtrot::TelemetryServer telemserv("",cl,2000);
  
  telemserv.BindSocket(bindstr);
  
  configure_telemetry_server(configfile,cl,telemserv);
  
  auto servtask = telemserv.runserver();
  
  auto except = servtask.get();
  if(except)
  {
    std::rethrow_exception(except);
  };

}
