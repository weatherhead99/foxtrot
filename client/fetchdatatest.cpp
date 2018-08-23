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
  
  std::vector<foxtrot::ft_variant> args {1259};
  auto dat = cl.FetchData(dummy_devid,"getCountStream",0,100,args.begin(),args.end());
  
  auto datvec = boost::get<std::vector<unsigned char>>(dat);
  cout << "data size: " << datvec.size() << endl;
  
  for(int i=0 ; i < 10; i++)
  {
   cout << (int) datvec[i] << " ";   
  }
  cout << endl;
  
  cout << "get double valued vector" << endl;
  std::vector<foxtrot::ft_variant> args2{500};
  auto dat2 = cl.FetchData(dummy_devid,"getRandomVector",0,100,args2.begin(), args2.end());
  
  cout << "get from variant.." << endl;
  auto dvec2 = boost::get<std::vector<double>>(dat2);
  cout << "data size: " << dvec2.size() << endl;
  
  for(int i=0; i < dvec2.size(); i++)
  {
   cout <<  dvec2[i] << " ";
      
  }
  cout << endl;

}
