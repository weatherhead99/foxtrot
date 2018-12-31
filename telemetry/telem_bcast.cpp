#include "TelemetryServer.h"
#include "telem_util.h"
#include <boost/program_options.hpp>
#include <backward.hpp>
#include <iostream>
#include <sstream>
#include "client.h"
#include "Logging.h"
#include "config.h"
#include <memory>


using std::cout;
using std::cerr;
using std::endl;


namespace po = boost::program_options;

int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  
  po::options_description desc("telemetry broadcaster for foxtrot telemetry.");
  foxtrot::setDefaultSink();
  
  std::string telemfile;
  int port = 0;
  std::string addr;
  int debuglevel = 0;
  std::string bindstr;
  std::string payload_type;
  std::string transport_type;
  std::string topic;
  
  foxtrot::Logging lg("telem_bcast");
  auto config_file = foxtrot::get_config_file_path("FOXTROT_TELEM_CONFIG", "telemetry.config");
  foxtrot::create_config_file(config_file);
  lg.strm(sl::info) << "config file path: " << config_file;
  
  int tick_ms;
  
  
  desc.add_options()
  ("telemfile,f",po::value<std::string>(&telemfile)->required(),"config file")
  ("port,p",po::value<int>(&port)->default_value(50051),"port to connect to")
  ("addr,a", po::value<std::string>(&addr)->default_value("0.0.0.0"),
            "address of foxtrot server")
  ("debuglevel,d", po::value<int>(&debuglevel)->default_value(3),"debugging output level")
  ("bindstr,b",po::value<std::string>(&bindstr)->default_value("tcp://*:50052"),
            "bind string for server")
  ("payload_type,m", po::value<std::string>(&payload_type)->default_value("protobuf"),
            "telemetry message payload type (protobuf or json)")
  ("transport_type,c", po::value<std::string>(&transport_type)->default_value("nanomsg"),
            "transport type for telemetry (nanomsg or mqtt)")
  ("tickms,t",po::value<int>(&tick_ms)->required(), "tick time in ms")
  ("nanomsg_topic,nt",po::value<std::string>(&topic)->default_value(""),
   "master topic for nanomsg transport")
  ;

  po::positional_options_description pdesc;
  pdesc.add("telemfile",-1);
  
  
  po::variables_map vm;
  
  lg.strm(sl::debug) << "options setup, about to open config file...";
  
  po::store(po::command_line_parser(argc,argv).options(desc).positional(pdesc).run(), vm);
  
  foxtrot::load_config_file(config_file, desc,vm,&lg);
  
  try{
    po::notify(vm);
  }
  catch(boost::program_options::required_option& err)
  {
      lg.strm(sl::fatal) << "required option missing...";
      lg.strm(sl::fatal) << err.what();
      std::exit(1);
  }
  
  foxtrot::check_debug_level_and_exit(debuglevel, lg);
  
  std::ostringstream oss;
  oss << addr << ":" << port;

  foxtrot::Client cl(oss.str());
  
  auto transport = std::make_unique<foxtrot::NanomsgTransport>(topic);
  transport->BindSocket(bindstr);
  
  foxtrot::TelemetryServer telemserv(cl,std::move(transport),2000);
  
  configure_telemetry_server(telemfile,cl,telemserv);
  
  auto servtask = telemserv.runserver();
  
  auto except = servtask.get();
  if(except)
  {
    std::rethrow_exception(except);
  };

}
