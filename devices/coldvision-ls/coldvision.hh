#pragma once
#include <foxtrot/CmdDevice.h>
#include <rttr/registration>
#include <foxtrot/protocols/SerialProtocol.h>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <string>

namespace foxtrot
{
  namespace devices
  {
    using std::shared_ptr;
    using foxtrot::SerialProtocol;
    using std::string;

    
    enum class LEDEqualizerStatus : short unsigned
      {
	None_Stable = 0,
	Locked = 1,
	WaitingForDelay = 2,
	IntensityLow = 4,
	IntensityHigh = 6,
	OverRange = 8,
	UnderRange = 10
      };

    enum class LEDTriggerMode : short unsigned
      {
	CW,
	ContinuousStrobe,
	TriggeredStrobe
      };

    enum class ColdVisionStatus : unsigned char
      {
	GOOD =1,
	WARNING =2,
	ERROR = 3
      };
    
    //NOTE: just supporting legacy comms for now, should be good enough
    class ColdVisionLS : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)

      public:
      ColdVisionLS(shared_ptr<SerialProtocol> proto);
      ~ColdVisionLS() override;

      string cmd(const string& request) override;
      
      //TODO: implement Optional return types
      float get_boardTemp();
      float get_LEDTemp();

      float get_inputVoltage();
      float get_referenceVoltage();

      unsigned get_fanSpeed();

      LEDEqualizerStatus get_equalizerStatus() ;
      unsigned get_lightFeedbackRaw();


      unsigned char get_errorFlags();

      unsigned get_analogIn(unsigned short idx);
      bool get_digitalIn(unsigned short idx);

      
      string get_ProductName();
      string get_FirmwareVersion();
      string get_SerialNumber();
      string get_ModelNumber();

      bool get_demoMode();
      void set_demoMode(bool onoff);

      bool get_combinedTrigger();
      void set_combinedTrigger(bool onoff);

      unsigned char get_knobMode();
      void set_knobMode(unsigned char mode);

      bool get_singleChannel();
      void set_singleChannel(bool onoff);

      bool get_DIOActiveHigh(unsigned char idx);
      void set_DIOActiveHigh(unsigned char idx, bool onoff);

      bool get_onOff(unsigned char idx);
      void set_onOff(unsigned char idx, bool onoff);

      unsigned short get_powerLimit(unsigned char idx);
      void set_powerLimit(unsigned char idx, unsigned short power);

      LEDTriggerMode get_triggerMode();
      void set_triggerMode(LEDTriggerMode mode);
      
      unsigned get_continuousStrobeFrequency();
      void set_continuousStrobeFrequency(unsigned freq);

      unsigned short get_continuousStrobeOffset(unsigned char idx);
      void set_continuousStrobeOffset(unsigned char idx, unsigned short offset);

      unsigned short get_continuousStrobeDutyCycle(unsigned char idx);
      void set_continuousStrobeDutyCycle(unsigned char idx, unsigned short offset);
      
      bool get_continuousStrobePolarity(unsigned char idx);
      void set_continuousStrobePolarity(unsigned char idx, bool onoff);

      unsigned get_triggeredStrobeDelay(unsigned char idx);
      void set_triggeredStrobeDelay(unsigned char idx, unsigned delay_us);

      unsigned get_triggeredStrobeOnTime(unsigned char idx);
      void set_triggeredStrobeOnTime(unsigned char idx, unsigned ontime_us);

      bool get_triggeredStrobeFallingEdge(unsigned char idx);
      void set_triggeredStrobeFallingEdge(unsigned char idx, bool onoff);

      bool get_equalizer();
      void set_equalizer(bool onoff);

      unsigned short get_startUpEqualizerDelay();
      void set_startUpEqualizerDelay(unsigned short delay);

      unsigned short get_equalizerTarget();
      void set_equalizerTarget(unsigned short target);

      unsigned short get_currentLightOutput();
      unsigned short get_equalizerOutput();

      int get_fanSpeedSetPoint();
      void set_fanSpeedSetPoint(int setpoint);

      bool Reconnect() override;


      private:

      void check_idx(unsigned char idx);
      

      template<typename T> void set_helper(const string& cmd, T value, bool hex=false)
      {
	std::ostringstream oss;
	if(hex)
	  oss << std::hex;
	oss << cmd << value << "\r";
	auto cmdstr = oss.str();
	this->cmd(cmdstr);
      }

      template<typename T1, typename T2>
      string two_parameter_command_helper(const string& cmd, T1&& value1, T2&& value2)
      {
	std::ostringstream oss;
	oss << cmd << static_cast<unsigned int>(value1) << ",";
	if constexpr(std::is_same_v<std::decay_t<T2>, char>)
	  {
	    oss << value2;
	  }
	else
	  {
	    oss << static_cast<unsigned int>(value2);
	  }
	oss << "\r";

	return oss.str();

      }

	

      string query_cmd(const string& req);
      float temp_query_helper(char tchar, const string& errstr);
      
      foxtrot::Logging _lg;
    };

    
    
  }


  
}
