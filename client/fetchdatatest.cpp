#include "client.h"
#include <iostream>
#include "Logging.h"

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
  
//   auto dat = cl.FetchData(dummy_devid,"getCountStream",0,100);
  


}