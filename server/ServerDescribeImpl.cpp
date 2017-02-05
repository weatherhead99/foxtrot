#include "ServerDescribeImpl.h"
#include <iostream>

foxtrot::ServerDescribeLogic::ServerDescribeLogic(const std::string& comment, foxtrot::DeviceHarness& harness)
: _servcomment(comment), _harness(harness)
{
}


void foxtrot::ServerDescribeLogic::HandleRequest(reqtp& req, repltp& repl)
{
  
      std::cout << "processing server describe request" << std::endl;
      
      repl.set_servcomment(_servcomment);
      
      auto devmap = _harness.GetDevMap();
      
      auto outdevmap = repl.mutable_devs_attached();
      
      for(auto& devpair : devmap)
      {
          auto devid = devpair.first;
          
          devdescribe desc;
          desc.set_devid(devid);
          desc.set_devtype(devpair.second->getDeviceTypeName());
          desc.set_devcomment(devpair.second->getDeviceComment());
          (*outdevmap)[devid] = desc;    
      };
        
    
}
