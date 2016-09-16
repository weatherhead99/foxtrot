#include "ProtocolUtilities.h"

#include <sstream>
#include <algorithm>


std::string foxtrot::read_until_endl(foxtrot::SerialProtocol* proto, unsigned int readlen, char endlchar)
{
  //TODO: functionality to test how many reads were required
//   std::cout << "tp1" << std::endl;;
  auto ret = proto->read(readlen);
  
  std::cout << "first ret: " << ret << " size: " <<ret.size() << std::endl;
  
  
  decltype(ret.begin()) endlpos;
  
  while( (endlpos = std::find(ret.begin(), ret.end(), endlchar) )  == ret.end())
  {
    std::cout << "need to read more" << std::endl;
    ret += proto->read(readlen);
  }
  
//   std::cout << "tp2" << std::endl;;
//   std::cout << "repl size: " << ret.size() << std::endl;
  
  
  
  return std::string(ret.begin(), endlpos - 1);
  
  
}
