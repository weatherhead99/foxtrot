#pragma once
#include <memory>
#include <string>

#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/SerialProtocol.h>
#include <iostream>


using std::string;

namespace foxtrot
{
  namespace protocols
  {
    class SerialPort;
    class BulkUSB;
  }

  
  namespace devices
  {
    
    enum class powerunits : short unsigned
    {
      
     Amps = 0,
     Volts = 1,
     Watts = 2,
     Watts_cm2 = 3,
     Joules = 4,
     Joules_cm2 = 5
    };
    
    enum class powermodes : short unsigned
    {
      DC_cont =0,
      DC_sing = 1,
      Integrate = 2,
      PP_cont = 3,
      PP_sing = 4,
      pulse_cont = 5,
      pulse_single = 6,
      RMS = 7      
    };

    enum class powermeterimpedance : short unsigned
      {
	_50_ohm = 0,
	_100_kohm = 1,
	_1_megohm = 2
      };

    enum class powermeteroutputrange : short unsigned
      {
	_1_V  = 0,
	_2_V = 1,
	_5_V = 2,
	_10_V = 3
      };

    enum class powermeterfiltermodes: short unsigned
      {
	NO_FILTER = 0,
	ANALOG_FILTER = 1,
	DIGITAL_FILTER = 2,
	ANALOG_AND_DIGITAL_FILTER = 3
      };

    enum class powermeterexternaltriggermodes : short unsigned
      {
	DISABLED = 0,
	CHANNEL_1 = 1,
	CHANNEL_2 = 2,
	BOTH_CHANNELS = 3
      };

    enum class powermetertriggerrunmodes : short unsigned
      {
	FOREVER = 0,
	EXTERNAL_TRIGGER = 1,
	SOFT_KEY = 2,
	SOFTWARE = 3
      };

    enum class powermeteranalogfilterfreq : short unsigned
      {
	NONE = 0,
	_250_kHz = 1,
	_12_5_kHz = 2,
	_1_kHz = 3,
	_5_Hz = 4
      };
    
    namespace detail {

      template<typename T>
      concept Enumeration = std::is_enum_v<T>;

      template<typename T> struct always_false :
	std::false_type {};
      
      template<typename T> struct return_converter
      {
	static T get(const std::string& in) { static_assert(always_false<T>::value);};
      };
      
      template<> struct return_converter<int>
      {
	static int get(const std::string& in) {

	  return std::stoi(in);};
      };
      
      template<> struct return_converter<double>
      {
	static double get(const std::string& in) {
	  return std::stod(in);};
      };
      
      template<Enumeration T> struct return_converter<T>
      {
	static T get(const std::string& in)
	{
	  return static_cast<T>(std::stoi(in));
	};

      };
    }
    
    class newport2936R : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)
    public:
      newport2936R(std::shared_ptr<SerialProtocol> proto);
      newport2936R(std::shared_ptr<foxtrot::protocols::SerialPort> proto);
      newport2936R(std::shared_ptr<foxtrot::protocols::BulkUSB> proto);
      
      const std::string getDeviceTypeName() const override;
      std::string cmd(const std::string& request) override;
      void cmd_no_response(const std::string& request) override;

      std::string info();
      int getErrorCode();
      std::string getErrorString();
     
      //------------ implementations in reference manual order -----------------------
      powermeteranalogfilterfreq getAnalogFilter();
      void setAnalogFilter(powermeteranalogfilterfreq value);

      powermeterimpedance getOutputImpedance();
      void setOutputImpedance(powermeterimpedance imp);

      powermeteroutputrange getOutputRange();
      void setOutputRange(powermeteroutputrange range);
      //TODO: output range!

      bool getAttenuator();
      std::string getAttenuatorSerialNumber();

      bool getAutoRange();
      void setAutoRange(bool onoff);
      
      std::string getcaldate();
      double getcaltemp();

      int getChannel();
      void setChannel(int chan);

      std::array<double, 3> getCorrection();
      void setCorrection(double d1, double d2, double d3);
      
      std::string getDetectorModel();
      

      double getArea(); //this is DETSIZE? in manual
      string getSerialNumber();

      int getDigitalFilter();
      void setDigitalFilter(int value);
      
      //NOTE: missing DPower as it seems confusing...

      //datastore related stuff
      bool getBufferBehaviour();
      void setBufferBehaviour(bool mode);
      void clearDataStore();
      int getDataStoreCount();
      bool getDataStoreEnable();
      void setDataStoreEnable(bool onoff);
      double getDataStoreValue(int idx);
      std::vector<double> fetchDataStore(int begin, int end);
      std::vector<double> fetchDataStoreOldest(int n);
      std::vector<double> fetchDataStoreNewest(int n);
      int getDataStoreInterval();
      void setDataStoreInterval(int interval);
      //not doing save buffer to file
      int getDataStoreSize();
      void setDataStoreSize(int size);
      foxtrot::devices::powerunits getDataStoreUnits();
      void setDataStoreUnits(foxtrot::devices::powerunits units);


      powermeterfiltermodes getFilterMode();
      void setFilterMode(powermeterfiltermodes mode);

      //TODO: filter frequency??

      void setLambda(int l);
      int getLambda();
      std::tuple<int,int>  getLambdaRange();
      std::tuple<double, double> getPowerRange();
      //TODO: measurement timeout, don't think we need, it's for pulses
      powermodes getMode();
      void setMode(powermodes mode);      

      double getPower();
      //NOTE: leaveing PWS, don't need it.

      void setRange(int r);
      int getRange();
      //TODO reference? probably not needed
      double getResponsivity();

      bool getRun();
      void setRun(bool onoff);
      
      //TODO: spot size??
      //TODO: statistics? probably not needed

      double getTemperature();
      void setExternalTriggerMode(int mode);
      int getExternalTriggerMode();

      void setTriggerEdge(int edge);
      int getTriggerEdge();

      //TODO: trigger holdoff?

      void setTriggerStartMode(powermetertriggerrunmodes mode);
      powermetertriggerrunmodes getTriggerStartMode();

      void setTriggerEndMode(powermetertriggerrunmodes mode);
      powermetertriggerrunmodes getTriggerEndMode();

      bool getTriggerState();
      void manualTriggerState(bool state);
      void setTriggerValue(double meas_val);
      double getTriggerValue();
      void setTriggerTimeout(int time_ms);
      int getTriggerTimeout();

      powerunits getUnits();
      void setUnits(powerunits unit);

      //TODO: zero value? low priority


    private:

      void clear_errors();
      void flush_buffers_after_timeout(int n);

      std::vector<double> do_fetch_buffer(int n);
      // std::string fetch_store_buffer();
      
      void check_and_throw_error();
      
      
      foxtrot::Logging _lg;
      void strip_CRLF(std::string& buffer);
      
      std::shared_ptr<SerialProtocol> _proto;
      bool _usbmode;

      void setup_usb_mode();
      void setup_serial_mode();

      std::string cmd_usb_mode(const std::string& request);
      std::string cmd_serial_mode(const std::string& request);

      std::string usb_read_line();
      std::string serial_read_line();

      using MFP = std::string (newport2936R::*)(const std::string&);
      using MFP2 = std::string (newport2936R::*)();
      
      MFP cmdptr = nullptr;
      MFP2 readlineptr = nullptr;

      template<typename T> void command_write(const std::string& cmd, 
					      const T& arg)
	{
	  std::ostringstream oss;
	  oss << cmd << " " << arg << '\r';
	  cmd_no_response(oss.str());
	  check_and_throw_error();
	};

      template<detail::Enumeration T> void command_write(const std::string& cmd,
						 const T& arg)
      {
	command_write<short unsigned>(cmd, static_cast<short unsigned>(arg));
      }
	
      template<typename T> T command_get(const std::string&& req)
      {
        auto str = cmd(req);
	check_and_throw_error();
        
        T out;

	_lg.strm(sl::trace) <<"return was: [" << str << "]";
	
        try{
            out = detail::return_converter<T>::get(str);
        }
        catch(std::invalid_argument& err)
        {
            _lg.strm(sl::error) << " error return from request: " << req;
        };
	
        return out;
	
      }
	
	

    };
    
    

  }//namespace devices
  
}//namespace foxtrot

std::vector<double> parse_datastore_string(const std::string& in);

std::string convert_powerunit_to_string(foxtrot::devices::powerunits p, bool& ok);
