#include "ssc_tcube_edge.hh"
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <rttr/registration>


#include <memory>
#include <optional>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <iomanip>
#include <bitset>


const foxtrot::parameterset TCube_serial_params
  {
    {"baudrate", 57600}
  };


namespace foxtrot
{

  using foxtrot::protocols::SerialPort;
  using std::static_pointer_cast;
  
  namespace devices
  {
   
    struct TCubeEdgeImpl
    {
      using TClock = std::chrono::steady_clock;
      using TTime = std::chrono::time_point<TClock>;

      std::optional<TTime> last_command_sent = std::nullopt;
      std::unique_ptr<foxtrot::Logging> lg;

    };

    TCubeEdge::~TCubeEdge() = default;

    TCubeEdge::TCubeEdge(shared_ptr<SerialPort> proto) :
      CmdDevice(proto)
    {
      proto->Init(&TCube_serial_params);
      impl = std::make_unique<TCubeEdgeImpl>();
      impl->lg = std::make_unique<foxtrot::Logging>("TCubeEdge");
      proto->flush();

    }

    string TCubeEdge::cmd(const string& request)
    {

      
      cmd_no_response(request);

      auto serproto = static_pointer_cast<SerialPort>(_proto);
      auto resp = serproto->read_until_endl(char(0x0d));

      auto endlpos = resp.find(char(0x0d));
      if(endlpos == std::string::npos)
	return resp;

      return resp.substr(0,endlpos);
    }

    void TCubeEdge::cmd_no_response(const string& request)
    {
      if(impl->last_command_sent.has_value())
	{
	  impl->lg->strm(sl::debug) << "checking command timeout...";
	  auto now = TCubeEdgeImpl::TClock::now();
	  auto next_possible_command_time = *(impl->last_command_sent) + std::chrono::milliseconds(200);
	  
	  while( next_possible_command_time > now)
	    {
	      impl->lg->strm(sl::debug) << "need to wait before sending command";
	      std::this_thread::sleep_until(next_possible_command_time);
	      now = TCubeEdgeImpl::TClock::now();
	    }
	}

      
      string out;

      //NOTE: consider a mutating version of cmd()??
      string actreq = request;
      auto serproto = static_pointer_cast<SerialPort>(_proto);
      serproto->flush();

      if(request.size() == 0)
	throw std::logic_error("can't use an empty request, this is a programming error");

      auto endlpos = actreq.find(char(0x0d));
      if(endlpos == std::string::npos)
	{
	  impl->lg->strm(sl::trace) << "adding missing EOL character";
	  actreq += char(0x0d);
	}
      else if(endlpos != actreq.size() -1)
	{
	  impl->lg->strm(sl::warning) << " EOL character found but it is not at end of string, discarding the rest";
	  actreq = actreq.substr(0, endlpos+1);
	}

      impl->lg->strm(sl::trace) << "request is: [" << actreq << "]";

      auto timeout = serproto->calc_minimum_transfer_time(actreq.size()) + std::chrono::milliseconds(100);
      impl->lg->strm(sl::debug) << "timeout for transfer is: " << timeout.count()*10 << "ms";

      serproto->setWait(timeout.count()*10);

      serproto->write(actreq);
      impl->last_command_sent = devices::TCubeEdgeImpl::TClock::now();


    }

    
    string TCubeEdge::identify()
    {
      return cmd("IDN");
    }

    void TCubeEdge::startstop(bool onoff)
    {
      switch(onoff)
	{
	case(false): cmd_no_response("STOP"); break;
	case(true): cmd_no_response("RUN"); break;
	}
	   
    }

    bool TCubeEdge::running()
    {
      auto resp = cmd("RUN?");

      if(resp == "RUNNING")
	return true;
      else if(resp == "STOPPED")
	return false;
      else
	{
	  impl->lg->strm(sl::error) << "expected running or stopped, received: [" << resp << "]";
	  throw std::runtime_error("invalid response received for is_started()");
	}
    }

    double TCubeEdge::RTD_temp()
    {
      auto resp = cmd("TEMP?");
      return std::stod(resp);
    }

    double TCubeEdge::pump_temp()
    {
      auto resp = cmd("PUMPTEMP?");
      return std::stod(resp);
      
    }

    double TCubeEdge::setpoint_temp()
    {
      auto resp = cmd("SETTEMP?");
      return std::stod(resp);
    }
    
    void TCubeEdge::set_backlight(bool onoff)
    {
      switch(onoff)
	{
	case(true): cmd_no_response("BLON"); break;
	case(false): cmd_no_response("BLOFF"); break;
	}
    }

    double TCubeEdge::RTD_offset()
    {
      auto resp = cmd("RTDOFFSET?");
      return std::stod(resp);
    }
    

    void TCubeEdge::set_RTD_offset(double offset)
    {
      std::ostringstream oss;
      oss << "RTDOFFSET " << std::fixed << std::setprecision(1) << offset;
      //NOTE: contrary to manual, this doesn't actually return a value on real hardware!
      
      cmd_no_response(oss.str());

      //impl->lg->strm(sl::debug) << "returned RTD offset is: " << resp;

    }

    void TCubeEdge::set_setpoint_temp(double temp)
    {
      std::ostringstream oss;
      oss << "SETTEMP " << std::fixed <<  std::setprecision(1) << temp;
      //NOTE: contrary to manual, this doesn't actually return a value on real hardware!
      cmd_no_response(oss.str());

      //impl->lg->strm(sl::debug) << "returned set point temperature is: " << resp;

    }

    double TCubeEdge::TE_PWM()
    {
      auto resp = cmd("PWM?");
      return std::stod(resp);
    }

    double TCubeEdge::fan_PWM()
    {

      //WARNING: does this work? It's undocumented?
      //NOTE: confirmed despite undocumented this does work!
      auto resp = cmd("FANPWM?");
      return std::stod(resp);
    }

    unsigned char TCubeEdge::statbits()
    {
      auto resp = cmd("STAT1A?");
      auto statul = std::stoul(resp);
      return statul & 0xFF;      

    }

    unsigned char TCubeEdge::faultbits()
    {
      auto resp = cmd("FLTS1A?");
      auto statul = std::stoul(resp);
      return statul & 0xFF;
    }

    bool TCubeEdge::warning()
    {
      std::bitset<8> bits(statbits());
      return bits[6];

    }

    bool TCubeEdge::system_alarm()
    {
      std::bitset<8> bits(statbits());
      return bits[7];
    }

    bool TCubeEdge::heating()
    {
      std::bitset<8> bits(statbits());
      return bits[5];
    }

    unsigned short TCubeEdge::temp_alarm()
    {
      std::bitset<8> bits(statbits());
      if(bits[3])
	return -1;
      else if(bits[4])
	return 1;
      return 0;
    }
    
    
    void TCubeEdge::restart()
    {
      cmd_no_response("RESTART");
    }

    std::vector<devices::TCubeEdgeFaults> TCubeEdge::faults()
    {

      std::vector<devices::TCubeEdgeFaults> out;
      
      auto flts = faultbits();
      std::bitset<8> bits(flts);
      out.reserve(bits.count());
      for(int i=0; i<8 ; i++)
	out.push_back(static_cast<TCubeEdgeFaults>(i));
	  
      return out;

    }
    
    double TCubeEdge::alarm_width()
    {
      auto resp = cmd("WIDTH?");
      return std::stod(resp);

    }

    void TCubeEdge::set_alarm_width(double width)
    {
      std::ostringstream oss;
      oss << "WIDTH " << std::fixed << std::setprecision(1) << width;

      //NOTE: contrary to manual, this doesn't actually return a value on real hardware!


      cmd_no_response(oss.str());
      //auto resp = cmd(oss.str());
      //impl->lg->strm(sl::debug) << "returned setpoint is: " << resp;
      
    }

    bool TCubeEdge::Reconnect()
    {
      auto serproto = static_pointer_cast<SerialPort>(_proto);
      //NOTE: this will throw if it fails 
      serproto->reconnect(std::nullopt);
      

      return true;
    }
    
  }

  } // namespace foxtrot


RTTR_REGISTRATION {
  using namespace rttr;

  using foxtrot::devices::TCubeEdge;
  using foxtrot::devices::TCubeEdgeFaults;

  registration::enumeration<TCubeEdgeFaults>("foxtrot::devices::TCubeEdgeFaults")
    ( value("RTDFault", TCubeEdgeFaults::RTDFault),
      value("TankLevelLowFault", TCubeEdgeFaults::TankLevelLowFault),
      value("PumpFailFault", TCubeEdgeFaults::PumpFailFault),
      value("FanFailFault", TCubeEdgeFaults::FanFailFault),
      value("CoolantRangeAlarmWarning", TCubeEdgeFaults::CoolantRangeAlarmWarning),
      value("PumpCurrentLowFault", TCubeEdgeFaults::PumpCurrentLowFault),
      value("PumpTempDiffExceededFault", TCubeEdgeFaults::PumpTempDiffExceededFault));

  registration::class_<TCubeEdge>("foxtrot::devices::TCubeEdge")
    .method("identify", &TCubeEdge::identify)
    .property_readonly("RTD_temp", &TCubeEdge::RTD_temp)
    .property_readonly("running", &TCubeEdge::running)
    .property_readonly("pump_temp", &TCubeEdge::pump_temp)
    .property("setpoint_temp", &TCubeEdge::setpoint_temp,
	      &TCubeEdge::set_setpoint_temp)
    .method("set_backlight", &TCubeEdge::set_backlight)
    (parameter_names("onoff"))
    .property("RTD_offset", &TCubeEdge::RTD_offset,
	      &TCubeEdge::set_RTD_offset)
    
    .property_readonly("TE_PWM", &TCubeEdge::TE_PWM)
    .property_readonly("fan_PWM", &TCubeEdge::fan_PWM)

    .property_readonly("warning", &TCubeEdge::warning)
    .property_readonly("system_alarm", &TCubeEdge::system_alarm)
    .property_readonly("heating", &TCubeEdge::heating)
    .property_readonly("temp_alarm", &TCubeEdge::temp_alarm)
    .property_readonly("statbits", &TCubeEdge::statbits)
    .property_readonly("faultbits", &TCubeEdge::faultbits)

    .method("restart", &TCubeEdge::restart)
    .property_readonly("faults", &TCubeEdge::faults)

    .property("alarm_width", &TCubeEdge::alarm_width,
	      &TCubeEdge::set_alarm_width)

    .method("startstop", &TCubeEdge::startstop)
    (parameter_names("onoff"))
    
    
    ;

}


