#pragma once
#include <foxtrot/CmdDevice.h>
#include <foxtrot/Logging.h>
#include <rttr/registration>
#include <memory>
#include <tuple>

namespace foxtrot
{

  namespace protocols
  {
    class SerialPort;
  }
  

  namespace devices
  {
    using std::shared_ptr;
    using foxtrot::protocols::SerialPort;

    enum class UserCalValue : short unsigned
      {
	ITE_SET_POINT = 1,
	ITE_MEAS = 2,
	VTE_MEAS = 3,
	THERMISTOR_10uA_MEAS = 4,
	THERMISTOR_100uA_MEAS = 5,
	THERMISTOR_1mA_MEAS = 6,
	LM335_MEAS = 7,
	AD950_MEAS = 8,
	RTD_100_MEAS = 9,
	RTD_100_4WIRE_MEAS = 10,
	//TODO: this one might be a typo in the manual!
	RTD_1000_MEAS = 11,
	VTE_4WIRE_MEAS = 12,
	ANALOG_OUTPUT_SET = 13,
	RTD_1000_4WIRE_MEAS = 14

      };

    enum class TECSensorType: unsigned char
      {
	DISABLED = 0,
	THERMISTOR_100uA = 1,
	THERMISTOR_10uA = 2,
	LM335 = 3,
	AD590 = 4,
	RTD_100 = 5,
	RTD_100_4WIRE = 6,
	RTD_1000 = 8,
	RTD_1000_4WIRE = 9

      };

    enum class TECControlMode : short unsigned
      {
	ITE_MODE,
	R_MODE,
	T_MODE
      };

    enum class TECHeatCool : short unsigned
      {
	BOTH,
	HEAT,
	COOL
      };

    enum class TECGain : short
      {
	ONE  = 1,
	THREE = 3,
	FIVE = 5,
	TEN = 10,
	THIRTY = 30,
	FIFTY = 50,
	ONE_HUNDRED = 100,
	THREE_HUNDRED = 300,
	PID = -1

      };

    

    
    class Arroyo5305 : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)

      public:
      Arroyo5305(shared_ptr<SerialPort> proto);
      std::string cmd(const std::string& request) override;

      void cmd_no_response(const std::string& request);

      std::string version();
      std::string identify();
      std::string serialno();


      float get_temp_setpoint();

      void set_temp_setpoint(float T);

      float temp();

      
      //TODO: in future, this could return a datetime??
      unsigned total_centiseconds_uptime();

      unsigned total_centiseconds_timer();

      bool get_remote_volt_sense();
      void set_remote_volt_sense(bool onoff);


      float voltage();

      float get_trate();
      void set_trate(float trate);

      std::tuple<float, float> get_tolerance();
      void set_tolerance(float tol, float time);

      unsigned char get_status();
      unsigned char get_eventstatus();
      std::vector<std::string> get_errors();

      std::array<float,3> get_PID();
      void set_PID(float P, float I, float D);

      bool get_output();
      void set_output(bool onoff);

      float get_vlimit();
      void set_vlimit(float V);

      float get_TLoLimit();
      void set_TLoLimit(float lim);
      
      float get_THiLimit();
      void set_THiLimit(float lim);

      float get_RLoLimit();
      void set_RLoLimit(float lim);

      float get_RHiLimit();
      void set_RHiLimit(float lim);

      float get_ITELimit();
      void set_ITELimit(float lim);
      
      float get_VLimit();
      void set_VLimit(float lim);

      //only available firmware 2.0 and later, disable for now
      // bool get_ITEInvert();
      // void set_ITEInvert(bool invert);

      float TECCurrent();

      void set_ITESetPoint(float I);
      float get_ITESetPoint();

      TECHeatCool get_heatcool();
      void set_heatcool(TECHeatCool val);
      //--------------impl to here ---------------//
      TECGain get_gain();
      void set_gain(TECGain gain);
      
      
      float resistance();
      
      std::tuple<float, unsigned short, unsigned short> get_fanspeed();
      void set_fanspeed(float speed, unsigned short mode,
			unsigned short delay);

      //void set_fanspeed(std::tuple<float, unsigned short, unsigned short> tplarg);

      unsigned char event();

      short unsigned get_outputoffregister();
      void set_outputoffregister(short unsigned reg);

      TECSensorType get_sensortype(int idx);
      void set_sensortype(int idx, TECSensorType tp);

      
    private:
      foxtrot::Logging _lg;

      void strip_crlf(std::string& str);
      
    };
  }
}
