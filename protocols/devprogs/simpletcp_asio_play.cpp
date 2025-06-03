#include <iostream>
#include <foxtrot/protocols/simpleTCP.h>
#include <foxtrot/Logging.h>
#include <memory>

using std::cout;
using std::endl;

using std::shared_ptr;
using std::string;

// parameters for starburst desktop
const string addr = "192.168.6.60";
const unsigned port = 4242u;



int main(int argc, char** argv)
{
  foxtrot::setLogFilterLevel(sl::trace);

  auto prot = std::make_shared<foxtrot::protocols::simpleTCPasio>(&addr, port);
  prot->Init();

}
