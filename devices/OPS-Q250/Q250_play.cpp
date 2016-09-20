#include <iostream>

#include "scsiserial.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  
  foxtrot::parameterset params{ {"devnode", "/dev/sdb"},
  {"timeout", 1000u},
  {"LBA", 100000u}
  };
  
  foxtrot::protocols::scsiserial proto(&params);
  
  proto.Init(nullptr);
  
  proto.write("IDN?");
  
  auto repl = proto.read(512);
  
  cout << "reply: "<< repl << endl;
  
  
}