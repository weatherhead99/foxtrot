#include <foxtrot/backward.hpp>
#include "stellarnet.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  //  foxtrot::devices::stellarnet spectro("/home/dweatherill/Software/stellarnet/files_to_copy/stellarnet.hex",1000);
  foxtrot::devices::stellarnet spectro(1000);

  cout << "reading spectrum..." << endl;

  auto dat1 = spectro.read_spectrum(800);
  
  std::ofstream ofs("spectrum.txt");
  
  for(auto& d : dat1)
  {
    
    ofs << d << " ";
  }

  cout << "reading 2nd spectrum..." << endl;
  auto dat2 = spectro.read_spectrum(400);
  
  
  

}
