#include <iostream>
#include <boost/dll/library_info.hpp>
#include <boost/dll/smart_library.hpp>

using std::cout;
using std::endl;


int main(int argc, char** argv)
{
  cout << "hello" << endl;
  auto linfo = boost::dll::library_info("./libplugin_example.so");

  for(auto& sec: linfo.sections())
    {
      cout << "section: " << sec << endl;
    }

  cout << "-----------------" << endl;
  for(auto& sym: linfo.symbols())
    {
      cout << "symbol: " << sym << endl;
    }
  
	
  
  
}
