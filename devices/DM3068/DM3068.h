#pragma once
#include <memory>
#include <sstream>
#include <iomanip>

#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/SerialProtocol.h>



using std::string;

namespace foxtrot {
  namespace devices {
    
    class DM3068  : public CmdDevice
    {
    public:
      DM3068(std::shared_ptr<SerialProtocol> proto);
      virtual ~DM3068();
      virtual std::string cmd(const std::string& request);
      string getIDNString();
      double get4WireResistance();
      
    private:
      std::shared_ptr<SerialProtocol> _serproto;
      
    };

  }
}
