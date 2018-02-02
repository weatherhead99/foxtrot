#include <iostream>
#include <client/client.h>
#include "client.h"
#include "archon/archon_module_heaterx.h"
#include <rttr/type>
#include "ServerUtil.h"

using std::cout;
using std::endl;



int main(int argc, char** argv)
{
  
  foxtrot::Client cl("localhost:50051");
  
  
  auto sd = cl.DescribeServer();
  auto devid = foxtrot::find_devid_on_server(sd,"ArchonHeaterX");
  
  cout << "devid: " << devid << endl;
  
  auto cap = foxtrot::find_capability(sd,devid,"getHeaterTarget");
  
  
//   auto argnames = sd.devs_attached().at(devid).caps()[cap].argnames();
//   auto argtypes = sd.devs_attached().at(devid).caps()[cap].argtypes();
  auto argnames = sd.devs_attached().at(devid).caps().Get(cap).argnames();
  auto argtypes = sd.devs_attached().at(devid).caps().Get(cap).argtypes();


  
  cout << argnames.Get(0) << endl;
  cout << argtypes.Get(0) << endl;
  
  auto fromtp = rttr::type::get<foxtrot::devices::HeaterXHeaters>();
  
  cout << fromtp.get_name() << endl;
  //OK, so definitely reporting string type, which is WRONG!!!
  
  auto value_type = foxtrot::get_appropriate_wire_type(fromtp);
  
  cout << "value_type: " << value_type << endl;
  
  rttr::variant var(fromtp);
  auto vartp = var.get_type();
  
  cout << vartp.get_name() << endl;
  
  auto var2 = fromtp.create();
  
  
  
  
  
  
  
  
  

};
