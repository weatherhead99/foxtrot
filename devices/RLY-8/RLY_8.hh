#pragma once

#include <string>
#include <vector>
#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using foxtrot::protocols::SerialPort;

namespace foxtrot
{

  
  namespace devices
  {

    enum class RLY_8_Commands : unsigned char
      {

      };

    class RLY_8 : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)

      public:
      RLY_8(shared_ptr<SerialPort> proto,
	    const std::string& comment);

      virtual ~RLY_8();

      string getName();
      void setName(const string& name);
      
      string getVersion();

      //not exposed over foxtrot public
      unsigned getBaudrate();
      void setBaudrate(unsigned brate);

      unsigned char getRelayState();

      bool getRelay(unsigned char channel);
      void setRelay(unsigned char channel);
      
    private:
      string assemble_cmd(unsigned char addr, RLY_8_Commands cmd,
			  const std::vector<unsigned char>& data);
      unsigned char calc_checksum(string::const_iterator start,
				  string::const_iterator end) const;
      
      
      string cmd(const std::string& request) override;

      vector<unsigned char> extract_response(const string& response,
					     unsigned char expd_addr,
					     RLY_8_Commands expd_cmd);
      
      foxtrot::Logging _lg;

    };
    
  }
  } // namespace foxtrot
  
