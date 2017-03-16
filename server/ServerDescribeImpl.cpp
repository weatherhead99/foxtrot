#include "ServerDescribeImpl.h"
#include <iostream>

foxtrot::ServerDescribeLogic::ServerDescribeLogic(const std::string& comment, foxtrot::DeviceHarness& harness)
: _servcomment(comment), _harness(harness), _lg("ServerDescribeLogic")
{
}


bool foxtrot::ServerDescribeLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, void* tag)
{
      _lg.Debug("processing server describe request");
      
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
          
          //enumerate capabilities
          //WARNING: SLOOOOW
          auto capnames = _harness.GetCapabilityNames(devid);
          for(auto& capname : capnames)
          {
//               cout << "adding capability: " << capname << endl;
              auto outcaps = desc.add_caps();
              outcaps->CopyFrom(_harness.GetDeviceCapability(devid, capname));
              
//               cout << outcaps->DebugString() << endl;
              
          }
          
          (*outdevmap)[devid] = desc;    
          
      };
        
     respond.Finish(repl,grpc::Status::OK,tag);
     return true;
}
