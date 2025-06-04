#include <iostream>
#include <foxtrot/protocols/simpleTCP.h>
#include <foxtrot/Logging.h>
#include <memory>
#include <chrono>
#include <thread>

using std::cout;
using std::endl;

using std::shared_ptr;
using std::string;
using namespace std::chrono_literals;

// parameters for starburst desktop
const string addr = "192.168.6.60";
const unsigned port = 4242u;
const std::chrono::milliseconds timeout=20ms;

void get_archon_status(auto proto, foxtrot::Logging& lg)
{
   lg.strm(sl::info) << "writing status request";
  proto->write(">01STATUS\n");

  auto response = proto->read_until_endl();

  cout << "response: " << response << endl;
  
  
}



int main(int argc, char** argv)
{
  foxtrot::setLogFilterLevel(sl::trace);

  foxtrot::Logging lg("simpletcp_asio_play");
  
  auto prot = std::make_shared<foxtrot::protocols::simpleTCPasio>(&addr, port, timeout);
  prot->Init();

  get_archon_status(prot, lg);

  cout << "closing port..." << endl;
  prot->close();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  cout << "another status request" << endl;

  get_archon_status(prot, lg);

}
