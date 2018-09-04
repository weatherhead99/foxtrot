#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"
#include <memory>
#include "Logging.h"
#include <string>
#include "DeviceError.h"

using std::string;

namespace foxtrot
{
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
    
    namespace detail {
      
      template<typename T> struct return_converter
      {
	static T get(const std::string& in) {};
      };
      
      template<> struct return_converter<int>
      {
	static int get(const std::string& in) {return std::stoi(in);};
      };
      
      template<> struct return_converter<double>
      {
	static int get(const std::string& in) {return std::stod(in);};
      };
      
      
    }
    
    class newport2936R : public CmdDevice
    {
      RTTR_ENABLE()
    public:
    newport2936R(std::shared_ptr< SerialProtocol> proto);
    virtual const std::string getDeviceTypeName() const override;
    virtual std::string cmd(const std::string& request);
    
    void setLambda(int l);
    int getLambda();

    int getErrorCode();
    std::string getErrorString();
    
    double getPower();
        
    double getResponsivity();
    double getArea();
    
#ifndef NEW_RTTR_API
    powerunits getUnits();
    void setUnits(powerunits unit);
#else
    int getUnits();
    void setUnits(int unit);
#endif
    
    
    double getTemperature();
    
    string getSerialNumber();
    
#ifndef NEW_RTTR_API
    powermodes getMode();
    void setMode(powermodes mode);
#else
    int getMode();
    void setMode(int mode);
#endif
    void manualTriggerState(bool state);
    bool getTriggerState();
    
    void setTriggerStartMode(int mode);
    int getTriggerStartMode();
    
    void setTriggerEndMode(int mode);
    int getTriggerEndMode();
    
    void setExternalTriggerMode(int mode);
    int getExternalTriggerMode();
    
    void setTriggerEdge(int edge);
    int getTriggerEdge();
    
    void setTriggerTimeout(int time_ms);
    int getTriggerTimeout();
    
    void setTriggerValue(double meas_val);
    double getTriggerValue();
    
    std::string getcaldate();
    double getcaltemp();
    
    int getChannel();
    void setChannel(int chan);
    
    int getFilterMode();
    void setFilterMode(int mode);
    
    int getDigitalFilter();
    void setDigitalFilter(int value);
    
    int getAnalogFilter();
    void setAnalogFilter(int value);
    
    bool getBufferBehaviour();
    void setBufferBehaviour(bool mode);
    
    int getDataStoreCount();
    
    int getDataStoreSize();
    void setDataStoreSize(int size);
    
    void clearDataStore();
    
    bool getDataStoreEnable();
    void setDataStoreEnable(bool onoff);
    
    foxtrot::devices::powerunits getDataStoreUnits();
    void setDataStoreUnits(foxtrot::devices::powerunits units);
    
    int getDataStoreInterval();
    void setDataStoreInterval(int interval);
    
    double getDataStoreValue(int idx);
    std::vector<double> fetchDataStore(int begin, int end);
    std::vector<double> fetchDataStoreOldest(int n);
    std::vector<double> fetchDataStoreNewest(int n);
    
    private:
      
      void flush_buffers_after_timeout(int n);
      
      std::string fetch_store_buffer();
      
      void check_and_throw_error();
      
      
      foxtrot::Logging _lg;
      void strip_CRLF(std::string& buffer);
      
      std::shared_ptr<SerialProtocol> _proto;
      bool _usbmode;

      template<typename T> void command_write(const std::string& cmd, 
					      const T& arg)
	{
	  std::ostringstream oss;
	  oss << cmd << " " << arg << '\r';
	  _proto->write(oss.str());
	};
	
      template<typename T> T command_get(const std::string& req)
      {
	std::ostringstream oss;
	auto str = cmd(req);
	
	try{
	  T out = detail::return_converter<T>::get(str);
	  return out;
	}
	catch(std::invalid_argument& err)
	{
	  _lg.strm(sl::error) << " error return from request: " << req;
	  check_and_throw_error();
	};
	
	
      }
	
	

    };
    
    

  }//namespace devices
  
}//namespace foxtrot

std::vector<double> parse_datastore_string(const std::string& in);

std::string convert_powerunit_to_string(foxtrot::devices::powerunits p, bool& ok);
