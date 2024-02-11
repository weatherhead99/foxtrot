#pragma once

#include <string>
#include <vector>
#include <foxtrot/CmdDevice.h>
#include <foxtrot/Logging.h>

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;

namespace foxtrot
{

  namespace protocols
  {
    class SerialPort;
  }
  
  namespace devices
  {

    using foxtrot::protocols::SerialPort;
    enum class RLY_8_Commands : unsigned char
      {
	QUERY_VERSION = 0x07,
	QUERY_RELAY_STATE = 0x01,
	SET_RELAY = 0x00
	
      };

    class RLY_8 : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)

      public:
      RLY_8(shared_ptr<SerialPort> proto,
	    const string& comment="RLY_8", unsigned char addr=0x01);

      virtual ~RLY_8();

      // manual appears inconsistent on this...
      //      string getName();
      //void setName(const string& name);
      
      string getVersion();

      //not exposed over foxtrot public
      unsigned getBaudrate();
      void setBaudrate(unsigned brate);

      unsigned char getRelayState();

      bool getRelay(unsigned char channel);
      void setRelay(unsigned char channel, bool onoff);
      
    private:
      string assemble_cmd(RLY_8_Commands cmd,
			  const std::vector<unsigned char>& data);

      template<typename It>
      unsigned char calc_checksum(It start,
				  It end) const
      {
	  unsigned sum = 0;
	  for( auto it = start; it != end; it++)
	    sum += *it;

	  return static_cast<unsigned char>(sum % 256);
      }
      
      
      string cmd(const std::string& request) override;

      vector<unsigned char> extract_response(const string& response,
					     RLY_8_Commands expd_cmd);
      
      foxtrot::Logging _lg;
      unsigned char _addr;

    };
    
  }
  } // namespace foxtrot
  
