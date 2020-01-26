#include "ServerDescribeImpl.h"
#include <iostream>

foxtrot::ServerDescribeLogic::ServerDescribeLogic(const std::string& comment, std::shared_ptr<foxtrot::DeviceHarness> harness)
: _servcomment(comment), _harness(harness), _lg("ServerDescribeLogic")
{
}


bool foxtrot::ServerDescribeLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
{
      _lg.Debug("processing server describe request");
      
      repl.set_servcomment(_servcomment);
      
      auto devmap = _harness->GetDevMap();
      
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
          auto capnames = devpair.second->GetCapabilityNames();
          for(auto& capname : capnames)
          {
              _lg.strm(sl::debug) << "adding capability: " << capname ;
              auto outcaps = desc.add_caps();
              outcaps->CopyFrom(_harness->GetDeviceCapability(devid, capname));
              
              _lg.strm(sl::trace) << "debug string: " << outcaps->DebugString();
              
              
          }
          
          (*outdevmap)[devid] = desc;    
          
      };
        
     respond.Finish(repl,grpc::Status::OK,tag);
     return true;
}
