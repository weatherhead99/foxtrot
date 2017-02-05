#include "ServerDescribeImpl.h"
#include <iostream>

void foxtrot::ServerDescribeLogic::HandleRequest(reqtp& req, repltp& repl)
{
  
      std::cout << "processing server describe request" << std::endl;
      
      repl.set_servcomment(_servcomment);
      
  
  
    
}
