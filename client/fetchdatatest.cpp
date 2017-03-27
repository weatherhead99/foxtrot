#include "client.h"
#include <iostream>
#include "Logging.h"
#include <backward.hpp>

using std::cout;
using std::endl;



int main(int argc, char** argv)
{
  
  foxtrot::setDefaultSink();
  foxtrot::Client cl("localhost:50051");
  
  auto servdescribe = cl.DescribeServer();
  auto dummy_devid = foxtrot::find_devid_on_server(servdescribe,"dummyDevice");
  
  cout << "connected to server: " << servdescribe.servcomment() << endl;
  cout << "dummy devid : " << dummy_devid << endl;
  
  std::vector<foxtrot::ft_variant> args {3000};
  auto dat = cl.FetchData(dummy_devid,"getCountStream",0,100,args.begin(),args.end());
  
  cout << "data size: " << dat.size() << endl;
  
  


}
