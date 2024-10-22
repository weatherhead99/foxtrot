#include "coldvision.hh"
#include <boost/math/policies/policy.hpp>
#include <boost/math/special_functions/detail/fp_traits.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <foxtrot/DeviceError.h>
#include <foxtrot/protocols/SerialPort.h>
#include <string>
#include <cctype>

using namespace foxtrot::devices;
using foxtrot::devices::ColdVisionLS;



template <typename Stream, typename T>
auto comma_stream(Stream& s, T&& arg) -> decltype(s << arg)
{
  return s << arg;
}

template <typename Stream, typename T, typename... Ts>
auto comma_stream(Stream& s, T&& arg, Ts... extra_args) -> decltype(s << arg)
{
  auto& newstream = s << arg << ",";
  return comma_stream<decltype(newstream), Ts...>(newstream, std::forward<Ts>(extra_args)...);
}

template <typename... Ts>
string comma_join(Ts... args)
{
  std::ostringstream oss;
  comma_stream(oss, args...);
  return oss.str();

}


ColdVisionLS::ColdVisionLS(shared_ptr<SerialProtocol> proto)
    : CmdDevice(proto), _lg("ColdVisionLS") {

  //WARNING: this will break (silently) on ethernet stuff
  std::static_pointer_cast<foxtrot::protocols::SerialPort>(proto)->setWait(100);
  proto->Init(nullptr);
  
}

ColdVisionLS::~ColdVisionLS() = default;

string ColdVisionLS::cmd(const string& request)
{

  auto serproto = std::static_pointer_cast<SerialProtocol>(_proto);
  
  if(request[0] != '&')
    throw std::logic_error("commands must start with & character");

  if(request.back() != '\r')
    throw std::logic_error("commands must end with a carriage return");

  serproto->write(request);

  auto resp = serproto->read_until_endl('\r');

  if(resp[0] != '&')
    throw std::logic_error("invalid first character received");

  if(resp[1] == 'n')
    {
      _lg.strm(sl::error) << "received negative ack from coldvision";
      _lg.strm(sl::error) << "response was: " << resp;
      throw foxtrot::DeviceError("invalid response code received from device, check logs...");

    }
  
  return resp;
}

string ColdVisionLS::query_cmd(const string& request)
{
  auto resp = cmd(request);

  auto querycharloc = request.find('?');
  if(querycharloc == std::string::npos)
    throw std::logic_error("couldn't find query character in string");

  string replystr;
  if(querycharloc == 1)
    {
      //this is a command like "&?BS\r" or similar
      replystr = resp.substr(request.size()-1,std::string::npos);
    }
  else if(querycharloc == (request.size() -2))
    {
      _lg.strm(sl::trace) << "query with a question mark at the end...";
      //this is a command like "&ES?\r" or similar
      replystr = resp.substr(querycharloc, std::string::npos);

    }
  else {
    throw std::logic_error("don't know how to deal with this query!");
  }

  return replystr;
  


}


float ColdVisionLS::temp_query_helper(char tchar, const string& errstr)
{
  std::ostringstream oss;
  oss << "&?" << tchar << "S\r";
  auto tmpgood = static_cast<bool>(std::stoi(query_cmd(oss.str())));
  if(not tmpgood)
    {
      oss.str("");
      oss << errstr << "temperature sensor problem!";
      throw foxtrot::DeviceError(oss.str());
    }

  oss.str("");
    oss << "&?" << tchar << "T\r";

  auto tmp = query_cmd(oss.str());
  return std::stod(tmp);
}




float ColdVisionLS::get_boardTemp()
{
  return temp_query_helper('B', "board");
}


float ColdVisionLS::get_LEDTemp()
{
  return temp_query_helper('L', "LED");
}


float ColdVisionLS::get_inputVoltage() { return std::stod(query_cmd("&?VI\r")); }

float ColdVisionLS::get_referenceVoltage() { return std::stod(query_cmd("&?VO\r"));}

unsigned ColdVisionLS::get_fanSpeed() { return std::stoi(query_cmd("&?G\r"));}

foxtrot::devices::LEDEqualizerStatus ColdVisionLS::get_equalizerStatus() {

  return static_cast<LEDEqualizerStatus>(std::stoi(query_cmd("&ES?\r")));
}

unsigned ColdVisionLS::get_lightFeedbackRaw()
{

  auto resp = query_cmd("&?I\r");
  _lg.strm(sl::trace) << "response to query: " << resp;

  //OK, so this one actually is hex
  return std::stoi(resp, nullptr, 16);
}

unsigned char ColdVisionLS::get_errorFlags()
{
  return std::stoi(query_cmd("&C?\r"));

}

unsigned ColdVisionLS::get_analogIn(unsigned short idx)
{
  auto queryfun = "&?a" + std::to_string(idx) + "\r";
  return std::stoi(query_cmd(queryfun));
}

bool ColdVisionLS::get_digitalIn(unsigned short idx)
{
  auto queryfun = "&?d" + std::to_string(idx) + "\r";
  return std::stoi(query_cmd(queryfun));
}


//NOTE: this one doesn't seem to work on our ColdVision-LS for now!
std::string ColdVisionLS::get_ProductName()
{
  return query_cmd("&Q?\r");
}

std::string ColdVisionLS::get_FirmwareVersion()
{
  auto fware_str =  query_cmd("&F?\r");
  fware_str.erase(std::remove_if(fware_str.begin(), fware_str.end(), ::isspace));
  return fware_str;
}


std::string ColdVisionLS::get_SerialNumber()
{
  auto sernum = query_cmd("&Z?\r");
  sernum.erase(std::remove_if(sernum.begin(), sernum.end(), ::isspace));
  
  return sernum;
}

std::string ColdVisionLS::get_ModelNumber()
{
  //WEIRD, the one listed in the manual doesn't work, oh well
  //for now we use the one which gives both model and serial number
  auto resp = query_cmd("&ZF?\r");

  auto splitpoint = resp.find(':');

  return resp.substr(0, splitpoint);
}


bool ColdVisionLS::get_demoMode() { return std::stoi(query_cmd("&D?\r")); }

void ColdVisionLS::set_demoMode(bool onoff)
{
  set_helper("&D", static_cast<int>(onoff));
}

bool ColdVisionLS::get_combinedTrigger()
{
  return std::stoi(query_cmd("&J0,?\r"));
}

void ColdVisionLS::set_combinedTrigger(bool onoff)
{
  set_helper("&J0,", static_cast<int>(onoff));
}

unsigned char ColdVisionLS::get_knobMode()
{
  return std::stoi(query_cmd("&N?\r"));
}

void ColdVisionLS::set_knobMode(unsigned char mode)
{
  set_helper("&N" , static_cast<int>(mode));
}

bool ColdVisionLS::get_singleChannel() {
  auto trigmode = get_triggerMode();
  string resp;
  switch(trigmode)
    {
    case(LEDTriggerMode::CW):  resp =  query_cmd("&B?\r"); break;
    case(LEDTriggerMode::TriggeredStrobe) : resp =  query_cmd("&PB?\r"); break;
    case(LEDTriggerMode::ContinuousStrobe) : resp =  query_cmd("&RM?\r"); break;  
    }

  return std::stoi(resp);
}


void ColdVisionLS::set_singleChannel(bool onoff)
{
  
  set_helper("&b", static_cast<int>(onoff));
  set_helper("&RB", static_cast<int>(onoff));
  set_helper("&PB", static_cast<int>(onoff));
}

bool ColdVisionLS::get_DIOActiveHigh(unsigned char idx)
{
  auto cmd = two_parameter_command_helper("&J", idx, '?');
  return std::stoi(query_cmd(cmd));

}


void ColdVisionLS::set_DIOActiveHigh(unsigned char idx, bool onoff)
{
  auto cmdstr = two_parameter_command_helper("&J", idx, onoff);
  cmd(cmdstr);
 
};


bool ColdVisionLS::get_onOff(unsigned char idx)
{
  auto cmdstr = two_parameter_command_helper("&L", idx, '?');
  return std::stoi(query_cmd(cmdstr));
}

void ColdVisionLS::set_onOff(unsigned char idx, bool onoff)
{
  auto cmdstr = two_parameter_command_helper("&L", idx, onoff);
  cmd(cmdstr);
}

unsigned short ColdVisionLS::get_powerLimit(unsigned char idx)
{
  auto cmdstr = two_parameter_command_helper("&i", idx, '?');
  return std::stoi(query_cmd(cmdstr));
 
}

void ColdVisionLS::set_powerLimit(unsigned char idx, unsigned short power)
{
  auto cmdstr = two_parameter_command_helper("&i", idx, power);
  cmd(cmdstr);
}

using foxtrot::devices::LEDTriggerMode;
LEDTriggerMode ColdVisionLS::get_triggerMode()
{
  bool trigtest = std::stoi(query_cmd("&RM?\r"));
  if(trigtest)
    return LEDTriggerMode::ContinuousStrobe;
  trigtest = std::stoi(query_cmd("&PM?\r"));
  if(trigtest)
    return LEDTriggerMode::TriggeredStrobe;

  return LEDTriggerMode::CW;

};

void ColdVisionLS::set_triggerMode(LEDTriggerMode mode)
{
  switch(mode)
    {
    case LEDTriggerMode::CW:
      cmd("&PM0\r");
      cmd("&RM0\r");
      return;
    case(LEDTriggerMode::ContinuousStrobe):
      cmd("&RM1\r");
      return;
    case(LEDTriggerMode::TriggeredStrobe):
      cmd("&PM1\r");
      return;
    }

}

unsigned ColdVisionLS::get_continuousStrobeFrequency()
{
  return std::stoi(query_cmd("&rf?\r"));
}

void ColdVisionLS::set_continuousStrobeFrequency(unsigned freq)
{
  if(freq < 6 or freq > 20000)
    throw foxtrot::DeviceError("out of bounds frequency specified (6-20000 allowed");

  set_helper("&rf", freq);
}


unsigned short ColdVisionLS::get_continuousStrobeOffset(unsigned char idx)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&rp", idx, '?');
  return std::stoi(query_cmd(cmdstr));
}

void ColdVisionLS::set_continuousStrobeOffset(unsigned char idx, unsigned short offset)
{

  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&rp", idx, offset);
  cmd(cmdstr);

}


unsigned short ColdVisionLS::get_continuousStrobeDutyCycle(unsigned char idx)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&rd", idx, '?');
  return std::stoi(query_cmd(cmdstr));
}


void ColdVisionLS::set_continuousStrobeDutyCycle(unsigned char idx, unsigned short offset)
{
  check_idx(idx);

  if(offset > 1000)
    throw foxtrot::DeviceError("invalid duty cycle, must be <= 1000");
  
  auto cmdstr = two_parameter_command_helper("&rd", idx, offset);
  cmd(cmdstr);

}

bool ColdVisionLS::get_continuousStrobePolarity(unsigned char idx)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&rj", idx, '?');
  return std::stoi(query_cmd(cmdstr));
}

void ColdVisionLS::set_continuousStrobePolarity(unsigned char idx, bool onoff)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&rj", idx, onoff);
  cmd(cmdstr);
  
}


unsigned ColdVisionLS::get_triggeredStrobeDelay(unsigned char idx)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&pd", idx, '?');
  return std::stoi(query_cmd(cmdstr));
}

void ColdVisionLS::set_triggeredStrobeDelay(unsigned char idx, unsigned delay_us)
{
  check_idx(idx);
  if(delay_us > 1000000)
    throw foxtrot::DeviceError("invalid triggered strobe delay, must be <1000000 us");

  auto cmdstr = two_parameter_command_helper("&pd", idx, delay_us);
  cmd(cmdstr);
}


unsigned ColdVisionLS::get_triggeredStrobeOnTime(unsigned char idx)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&po", idx, '?');
  return std::stoi(query_cmd(cmdstr));
}

void ColdVisionLS::set_triggeredStrobeOnTime(unsigned char idx, unsigned ontime_us)
{
  check_idx(idx);
  if(ontime_us > 1000000)
    throw foxtrot::DeviceError("invalid strobe on time, must be <1000000 us");
  auto cmdstr = two_parameter_command_helper("&po", idx, ontime_us);
  cmd(cmdstr);
}

bool ColdVisionLS::get_triggeredStrobeFallingEdge(unsigned char idx)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&pj", idx, '?');
  return std::stoi(query_cmd(cmdstr));
 
}

void ColdVisionLS::set_triggeredStrobeFallingEdge(unsigned char idx, bool onoff)
{
  check_idx(idx);
  auto cmdstr = two_parameter_command_helper("&pj", idx, onoff);
  cmd(cmdstr);
}

bool ColdVisionLS::get_equalizer()
{
  return std::stoi(query_cmd("&E?\r"));

}

void ColdVisionLS::set_equalizer(bool onoff)
{
  set_helper("&E", onoff);
}

unsigned short ColdVisionLS::get_startUpEqualizerDelay()
{
  return std::stoi(query_cmd("&EI?\r"));
}

void ColdVisionLS::set_startUpEqualizerDelay(unsigned short delay)
{
  set_helper("&EI", delay);
}

unsigned short ColdVisionLS::get_equalizerTarget() { return std::stoi(query_cmd("&EE?\r"), nullptr, 16); }

void ColdVisionLS::set_equalizerTarget(unsigned short target)
{
  if(target > 4095)
    throw foxtrot::DeviceError("invalid light target (allowed 0 - 4095)");

  set_helper("&EE", target, true);
}

unsigned short ColdVisionLS::get_currentLightOutput()
{
  return std::stoi(query_cmd("&EV?\r"), nullptr, 16);

}

unsigned short ColdVisionLS::get_equalizerOutput()
{
  return std::stoi(query_cmd("&ED?\r"), nullptr, 16);
}

int ColdVisionLS::get_fanSpeedSetPoint()
{
  bool fanManual = std::stoi(query_cmd("&ge?\r"));
  if(not fanManual)
    return -1;

  return std::stoi(query_cmd("&gs?\r"));
}

void ColdVisionLS::set_fanSpeedSetPoint(int setpoint)
{
  if(setpoint < 0)
    {
      set_helper("&ge", 0);
      return;
    }

  set_helper("&ge", 1);
  set_helper("&gs", setpoint);
}

  

  



void ColdVisionLS::check_idx(unsigned char idx)
{
  if(idx < 1 or idx > 4)
    throw foxtrot::DeviceError("invalid index for single channel operation (allowed 1-4)");
}


RTTR_REGISTRATION
{
  using namespace rttr;

 
  using foxtrot::devices::ColdVisionLS;
  using foxtrot::devices::LEDTriggerMode;
  using foxtrot::devices::LEDEqualizerStatus;

  registration::enumeration<LEDTriggerMode>("foxtrot::devices::LEDTriggerMode")
    ( value("CW", LEDTriggerMode::CW),
      value("ContinuousStrobe", LEDTriggerMode::ContinuousStrobe),
      value("TriggeredStrobe", LEDTriggerMode::TriggeredStrobe));

  registration::enumeration<LEDEqualizerStatus>("foxtrot::devices::LEDEqualizerStatus")
    ( value("None_Stable", LEDEqualizerStatus::None_Stable),
      value("Locked", LEDEqualizerStatus::Locked),
      value("WaitingForDelay", LEDEqualizerStatus::WaitingForDelay),
      value("IntensityLow", LEDEqualizerStatus::IntensityLow),
      value("IntensityHigh", LEDEqualizerStatus::IntensityHigh),
      value("OverRange", LEDEqualizerStatus::OverRange),
      value("UnderRange", LEDEqualizerStatus::UnderRange));

  registration::class_<ColdVisionLS>("foxtrot::devices::ColdVisionLS")
    .property_readonly("boardTemp", &ColdVisionLS::get_boardTemp)
    .property_readonly("LEDTemp", &ColdVisionLS::get_LEDTemp)
    .property_readonly("inputVoltage",&ColdVisionLS::get_inputVoltage)
    .property_readonly("referenceVoltage", &ColdVisionLS::get_referenceVoltage)
    .property_readonly("fanSpeed", &ColdVisionLS::get_fanSpeed)
    .property_readonly("equalizerStatus", &ColdVisionLS::get_equalizerStatus)
    .property_readonly("lightFeedbackRaw", &ColdVisionLS::get_lightFeedbackRaw)
    .property_readonly("errorFlags", &ColdVisionLS::get_errorFlags)
    .method("get_analogIn", &ColdVisionLS::get_analogIn)
    (parameter_names("idx"))
    .method("get_digitalIn", &ColdVisionLS::get_digitalIn)
    (parameter_names("idx"))

    //NOTE: disable this one, it's broken for now, we don't need it anyway
    //.property_readonly("ProductName", &ColdVisionLS::get_ProductName)
    .property_readonly("FirmwareVersion", &ColdVisionLS::get_FirmwareVersion)
    .property_readonly("SerialNumber", &ColdVisionLS::get_SerialNumber)
    .property_readonly("ModelNumber", &ColdVisionLS::get_ModelNumber)

    .property("demoMode", &ColdVisionLS::get_demoMode, &ColdVisionLS::set_demoMode)
    .property("combinedTrigger", &ColdVisionLS::get_combinedTrigger, &ColdVisionLS::set_combinedTrigger)
    .property("knobMode", &ColdVisionLS::get_knobMode, &ColdVisionLS::set_knobMode)
    .property("singleChannel", &ColdVisionLS::get_singleChannel, &ColdVisionLS::set_singleChannel)
    .method("get_DIOActiveHigh", &ColdVisionLS::get_DIOActiveHigh)
    (parameter_names("idx"))
    .method("set_DIOActiveHigh", &ColdVisionLS::set_DIOActiveHigh)
    (parameter_names("idx", "onoff"))
    .method("get_onOff", &ColdVisionLS::get_onOff)
    (parameter_names("idx"))
    .method("set_onOff", &ColdVisionLS::set_onOff)
    (parameter_names("idx","onoff"))
    .method("get_powerLimit", &ColdVisionLS::get_powerLimit)
    (parameter_names("idx"))
    .method("set_powerLimit", &ColdVisionLS::set_powerLimit)
    (parameter_names("idx", "power"))
    .property("triggerMode", &ColdVisionLS::get_triggerMode,
	      &ColdVisionLS::set_triggerMode)
    .property("continuousStrobeFrequency", &ColdVisionLS::get_continuousStrobeFrequency,
	      &ColdVisionLS::set_continuousStrobeFrequency)
    .method("get_continuousStrobeOffset", &ColdVisionLS::get_continuousStrobeOffset)
    (parameter_names("idx"))
    .method("set_continuousStrobeOffset", &ColdVisionLS::set_continuousStrobeOffset)
    (parameter_names("idx", "offset"))
    .method("get_continousStrobeDutyCycle", &ColdVisionLS::get_continuousStrobeDutyCycle)
    (parameter_names("idx"))
    .method("set_continuousStrobeDutyCycle", &ColdVisionLS::set_continuousStrobeDutyCycle)
    (parameter_names("idx", "offset"))
    .method("get_continuousStrobePolarity", &ColdVisionLS::get_continuousStrobePolarity)
    (parameter_names("idx"))
    .method("set_continuousStrobePolarity", &ColdVisionLS::set_continuousStrobePolarity)
    (parameter_names("idx", "onoff"))
    .method("get_triggeredStrobeDelay", &ColdVisionLS::get_triggeredStrobeDelay)
    (parameter_names("idx"))
    .method("set_triggeredStrobeDelay", &ColdVisionLS::set_triggeredStrobeDelay)
    (parameter_names("idx", "delay_us"))
    .method("get_triggeredStrobeOnTime", &ColdVisionLS::get_triggeredStrobeOnTime)
    (parameter_names("idx"))
    .method("set_triggeredStrobeOnTime", &ColdVisionLS::set_triggeredStrobeOnTime)
    (parameter_names("idx", "ontime_us"))
    .method("get_triggeredStrobeFallingEdge", &ColdVisionLS::get_triggeredStrobeFallingEdge)
    (parameter_names("idx"))
    .method("set_triggeredStrobeFallingEdge", &ColdVisionLS::set_triggeredStrobeFallingEdge)
    (parameter_names("idx", "onoff"))

    .property("equalizer", &ColdVisionLS::get_equalizer, &ColdVisionLS::set_equalizer)
    .property("startUpEqualizerDelay", &ColdVisionLS::get_startUpEqualizerDelay,
	      &ColdVisionLS::set_startUpEqualizerDelay)
    .property("equalizerTarget", &ColdVisionLS::get_equalizerTarget,
	      &ColdVisionLS::set_equalizerTarget)
    .property_readonly("currentLightOutput", &ColdVisionLS::get_currentLightOutput)
    .property_readonly("equalizerOutput", &ColdVisionLS::get_equalizerOutput)
    .property("fanSpeedSetPoint", &ColdVisionLS::get_fanSpeedSetPoint,
	      &ColdVisionLS::set_fanSpeedSetPoint)
     

    ;
    
  

}
