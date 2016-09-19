#include "ProtocolUtilities.h"

#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

// std::string foxtrot::read_until_endl(foxtrot::SerialProtocol* proto, unsigned readlen, char endlchar, unsigned wait_ms=0)
// {
//   //TODO: functionality to test how many reads were required
  
//   auto ret = proto->read(readlen);
//   decltype(ret.begin()) endlpos;
  
//   while( (endlpos = std::find(ret.begin(), ret.end(), endlchar) )  == ret.end())
//   {
//     std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

//     //std::cout << "need to read more" << std::endl;
//     auto this_repl = proto->read(readlen);
    


//     ret += proto->read(readlen);    

//   }
  
 
//   return std::string(ret.begin(), endlpos );
  
  
// }
