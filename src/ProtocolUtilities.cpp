#include "ProtocolUtilities.h"

#include <sstream>
#include <algorithm>


std::string foxtrot::read_until_endl(foxtrot::SerialProtocol* proto, unsigned int readlen, char endlchar)
{
  
  auto ret = proto->read(readlen);
  
  decltype(ret.begin()) endlpos;
  
  while( (endlpos = std::find(ret.begin(), ret.end(), endlchar) )  == ret.end())
  {
    std::cout << "need to read more" << std::endl;
    ret += proto->read(readlen);
  }
  
  return std::string(ret.begin(), endlpos - 1);
  
  
}
