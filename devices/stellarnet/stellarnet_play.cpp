#include <foxtrot/backward.hpp>
#include "stellarnet.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  //  foxtrot::devices::stellarnet spectro("/home/dweatherill/Software/stellarnet/files_to_copy/stellarnet.hex",1000);
  foxtrot::devices::stellarnet spectro("/home/weatherill/stellarnet.hex",1000);

  cout << "reading spectrum..." << endl;
  auto dat = spectro.read_spectrum(8000);
  
  std::ofstream ofs("spectrum.txt");
  
  for(auto& d : dat)
  {
    
    ofs << d << " ";
  }
  
  

}
