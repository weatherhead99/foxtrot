#include <iostream>
#include <foxtrot/protocols/simpleTCP.h>
#include <foxtrot/Logging.h>
#include <memory>
#include <chrono>

using std::cout;
using std::endl;

using std::shared_ptr;
using std::string;
using namespace std::chrono_literals;

// parameters for starburst desktop
const string addr = "192.168.6.60";
const unsigned port = 4242u;
const std::chrono::milliseconds timeout=20ms;


int main(int argc, char** argv)
{
  foxtrot::setLogFilterLevel(sl::trace);

  foxtrot::Logging lg("simpletcp_asio_play");
  
  auto prot = std::make_shared<foxtrot::protocols::simpleTCPasio>(&addr, port, timeout);
  prot->Init();

  lg.strm(sl::info) << "writing status request";
  prot->write(">01STATUS\n");

  
  

}
