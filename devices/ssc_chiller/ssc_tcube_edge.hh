#pragma once
#include <foxtrot/CmdDevice.h>
#include <rttr/registration>
#include <vector>

namespace foxtrot
{
  //TODO: separate command for heating cooling? Fan PWM?? TLL?
  
  namespace protocols
  {
    class SerialPort;
  }

  namespace devices
  {

    enum class TCubeEdgeFaults : short unsigned
      {
	RTDFault = 0b1,
	TankLevelLowFault = 0b10,
	PumpFailFault = 0b100,
	FanFailFault = 0b1000,
	CoolantRangeAlarmWarning = 0b10000,
	PumpCurrentLowFault = 0b1000000,
	PumpTempDiffExceededFault = 0b10000000
      };

    
    struct TCubeEdgeImpl;

    using std::shared_ptr;
    using std::unique_ptr;
    using foxtrot::protocols::SerialPort;
    using std::string;
    
    class TCubeEdge : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)

      public:
      TCubeEdge(shared_ptr<SerialPort> proto);
      ~TCubeEdge();

      string cmd(const string& request) override;

      string identify();

      void startstop(bool onoff);
      bool running();

      double RTD_temp();
      double pump_temp();

      double setpoint_temp();
      void set_setpoint_temp(double temp);

      void set_backlight(bool onoff);

      double RTD_offset();
      void set_RTD_offset(double offset);
      
      double TE_PWM();
      double fan_PWM();

      bool warning();
      bool system_alarm();
      bool heating();
      unsigned short temp_alarm();

      unsigned char statbits();
      unsigned char faultbits();
      
      void restart();

      std::vector<TCubeEdgeFaults> faults();

      double alarm_width();
      void set_alarm_width(double width);

      bool Reconnect() override;

      
    private:
      void cmd_no_response(const string& request);
      

      unique_ptr<TCubeEdgeImpl> impl;

    };

  }

}
    
