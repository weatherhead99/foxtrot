#pragma once
#include <foxtrot/CmdDevice.h>
#include <foxtrot/Logging.h>
#include <rttr/registration>

namespace foxtrot
{

  namespace protocols
  {
    class SerialPort;
  }

  namespace devices
  {

    enum class UC170_command_sigils : short unsigned
      {
    SET_POINT_TEMP = 0b1,
    ACT_TEMP = 0b1001,
    CHECK_FAULTS = 0b1000,
    PERCENT_POWER = 0b11110,
    RESET_ALARMS_RESTART = 0b11111
  };
    


    struct UC170Impl;
    
    using std::string;
    using std::shared_ptr;
    using foxtrot::protocols::SerialPort;
    
    class UC170Chiller : public CmdDevice
    {
      
      RTTR_ENABLE(CmdDevice)

    public:
      UC170Chiller(shared_ptr<SerialPort> proto);
      ~UC170Chiller();
      string cmd(const string& request) override;

      float get_ActualTemp();
      void set_SetPointTemp(float value);
      float get_SetPointTemp();
      float get_OutputPower();

      unsigned char get_Status();
      
      void reset_Errors();
      


      
    private:
      string prepare_command(UC170_command_sigils param, bool set=false);
      
      foxtrot::Logging _lg;
      std::unique_ptr<UC170Impl> _impl;

    };

  }


}
