#include "arroyo5305.hh"
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/DeviceError.h>

#include <memory>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

using boost::posix_time::duration_from_string;
using boost::posix_time::from_iso_string;

using namespace foxtrot;
using std::string;
using std::static_pointer_cast;


using foxtrot::devices::Arroyo5305;


const foxtrot::parameterset sport_defaults
  { {"baudrate" , 38400}
  };





template<typename Stream, typename T>
auto stream_all(Stream s, T&& arg) -> decltype( s << arg)
{
  return s << arg;
}



template<typename Stream, typename T, typename ...Ts>
auto  stream_all(Stream& s, T&& arg, Ts... extra_args) -> decltype( s << arg)
{
  auto& newstr = s << arg;
  return stream_all<decltype(newstr), Ts...>(newstr, std::forward<Ts>(extra_args)...);
}


template<typename ...Ts>
string construct_cmd(Ts... args)
{
  std::ostringstream oss;
  stream_all(oss, args...);

  return oss.str();
}


boost::posix_time::time_duration parse_arroyo_duration_string(const string& s, foxtrot::Logging* lg=nullptr)
{
  boost::posix_time::time_duration out;
  auto n_semicolons = std::count(s.begin(), s.end(),':');
  if(n_semicolons > 2)
    {
      if(lg)
	lg->strm(sl::debug) << "long time support enabled!";
      //long duration
      auto first_semi =  s.find(':');
      out = duration_from_string(s.substr(first_semi,std::string::npos));
      if(lg)
	lg->strm(sl::trace) << "time parsed: " << out;

      auto day_count = std::stoi(s.substr(0, first_semi));
      if(lg)
	lg->strm(sl::trace) << "day_count: " << day_count;

      out += boost::posix_time::hours(day_count*24);

      if(lg)
	lg->strm(sl::trace) << "total time: " << out;
    }
  else
    {

      out = duration_from_string(s);

    }

  return out;


}




Arroyo5305::Arroyo5305(shared_ptr<SerialPort> proto)
    : CmdDevice(proto), _lg("Arroyo5305")
{
  //  auto serproto = static_pointer_cast<SerialPort>(_proto);
  //turn off the terminal echo and make sure the termination character is '\r'


  //seems like we currently need this amount of wait, I don't like it.
  proto->setWait(1000);
  
  
  proto->Init(&sport_defaults);
  
  proto->write("TERM 2\r\n");
  proto->write("TERMINAL 0\r\n");

  //  proto->flush();

  //setup the status registers...
  proto->write("*ESE 255\r");
  proto->write("*SRE 255\r");
  
}


void Arroyo5305::cmd_no_response(const string& request)
{
  
  std::ostringstream reqfix;
  reqfix << request;
  
  //check last character is a line ending
  _lg.strm(sl::trace) << "request rbegin: " << std::hex << *(request.rbegin());
  
  if(! ( *(request.rbegin() ) == '\r' or *(request.rbegin()) == '\n'))
    {
      _lg.strm(sl::warning) << "request didn't end with a line ending character, this is a programming error, we will insert one please fix this bug though";
      reqfix << '\r';
    }

  auto serproto = static_pointer_cast<SerialPort>(_proto);
  
  serproto->write(reqfix.str());
   

}


string Arroyo5305::cmd(const string &request)
{
  cmd_no_response(request);
  auto serproto = static_pointer_cast<SerialPort>(_proto);
  auto resp = serproto->read_until_endl('\r');
  strip_crlf(resp);
  
  return resp;
}

std::string Arroyo5305::version()
{
  return cmd("VER?\r");
}


std::string Arroyo5305::identify()
{
  return cmd("*IDN?\r");
}

std::string Arroyo5305::serialno()
{
  return cmd("SN?\r");
}


float Arroyo5305::temp()
{
  return std::stod(cmd("TEC:T?\r"));
}

float Arroyo5305::get_temp_setpoint()
{
  return std::stod(cmd("TEC:SET:T?\r"));
}

void Arroyo5305::set_temp_setpoint(float T)
{
  auto val = construct_cmd("TEC:T " , T, "\r");
  _lg.strm(sl::trace) << "command is: " << val;
  cmd_no_response(val); 
}

unsigned Arroyo5305::total_centiseconds_uptime()
{
  auto resp = cmd("TIME?\r");

  _lg.strm(sl::trace) << "response: " << resp;

  auto time = parse_arroyo_duration_string(resp, &_lg);

  auto centisecs = time.total_milliseconds() / 10 ;
  
  return centisecs;

}

unsigned Arroyo5305::total_centiseconds_timer()
{
  auto resp = cmd("TIMER?\r");
  auto time = parse_arroyo_duration_string(resp, &_lg);
  auto centisecs = time.total_milliseconds() / 10;
  return centisecs;
}

bool Arroyo5305::get_remote_volt_sense() { return std::stoi(cmd("TEC:VSENSE?\r"));}
void Arroyo5305::set_remote_volt_sense(bool onoff) { cmd_no_response( construct_cmd("TEC:VSENSE ", onoff, "\r"));}

float Arroyo5305::voltage() { return std::stod(cmd("TEC:V?\r"));}

float Arroyo5305::get_trate() { return std::stod(cmd("TEC:TRATE?\r"));}
void Arroyo5305::set_trate(float trate) { cmd_no_response(construct_cmd("TEC:TRATE ", trate, "\r"));}

std::tuple<float, float> Arroyo5305::get_tolerance() {
  auto resp = cmd("TEC:TOL?\r");
  auto commapos = resp.find(',');
  if(commapos = std::string::npos)
    throw std::logic_error("couldn't find comma");

  float tolerance = std::stod(resp.substr(0, commapos));
  float time = std::stod(resp.substr(commapos+1, std::string::npos));

  return {tolerance, time};
}


void Arroyo5305::set_tolerance(float tolerance, float time)
{
  auto cmdstr = construct_cmd("TEC:TOL ", tolerance, ",", time, "\r");
  cmd_no_response(cmdstr);
}

unsigned char Arroyo5305::get_status() { return std::stoi(cmd("*STB?\r"));}
unsigned char Arroyo5305::get_eventstatus() { return std::stoi(cmd("*ESR?\r")); }



std::vector<string> Arroyo5305::get_errors() {

  auto resp = cmd("ERRSTR?\r");

  std::vector<string> out;
  std::istringstream iss;
  iss.str(resp);

  bool isnum=true;
  std::string st;
  for(bool oddeven=true; std::getline(iss, st, ',');oddeven = !oddeven)
    {
      if(isnum)
	continue;
      out.push_back(st);
    }
  
  return out;
}


std::array<float, 3> Arroyo5305::get_PID()
{
  auto resp = cmd("TEC:PID?\r");
  std::array<float, 3> out;
  std::istringstream iss(resp);
  std::string st;

  _lg.strm(sl::trace) << "response: " << resp;
  
  for(auto it= std::begin(out); std::getline(iss, st, ','); it++)
    *it = std::stof(st);

  return out;
      
}

void Arroyo5305::set_PID(float P, float I, float D)
{
  cmd_no_response(construct_cmd("TEC:PID ", P, I, D, "\r"));
}

bool Arroyo5305::get_output()
{
  return std::stoi(cmd("TEC:OUT?\r"));
}

void Arroyo5305::set_output(bool onoff)
{
  cmd_no_response(construct_cmd("TEC:OUT ", onoff, "\r"));
}


float Arroyo5305::get_TLoLimit(){ return std::stod(cmd("TEC:LIM:TLO?\r"));}
void Arroyo5305::set_TLoLimit(float lim){ cmd_no_response(construct_cmd("TEC:LIMIT:TLO ", lim, "\r"));}

float Arroyo5305::get_THiLimit(){ return std::stod(cmd("TEC:LIM:THI?\r"));}
void Arroyo5305::set_THiLimit(float lim){cmd_no_response(construct_cmd("TEC:LIMIT:THI ", lim, "\r"));}

float Arroyo5305::get_RLoLimit(){ return std::stod(cmd("TEC:LIM:RLO?\r"));}
void Arroyo5305::set_RLoLimit(float lim){ cmd_no_response(construct_cmd("TEC:LIMIT:RLO ", lim, "\r"));}

float Arroyo5305::get_RHiLimit(){ return std::stod(cmd("TEC:LIM:RHI?\r"));}
void Arroyo5305::set_RHiLimit(float lim) {cmd_no_response(construct_cmd("TEC:LIMIT:RHI ", lim, "\r"));}

float Arroyo5305::get_ITELimit(){ return std::stod(cmd("TEC:LIM:ITE?\r"));}
void Arroyo5305::set_ITELimit(float lim) {cmd_no_response(construct_cmd("TEC:LIMIT:ITE ", lim, "\r"));}

float Arroyo5305::get_VLimit(){ return std::stod(cmd("TEC:LIM:V?\r"));}
void Arroyo5305::set_VLimit(float lim){cmd_no_response(construct_cmd("TEC:LIMIT:V ", lim, "\r"));}


// bool Arroyo5305::get_ITEInvert(){ return std::stoi(cmd("TEC:INVERTITE?\r"));}
// void Arroyo5305::set_ITEInvert(bool invert){ cmd_no_response(construct_cmd("TEC:INVERTITE ",invert, "\r"));}

float Arroyo5305::TECCurrent(){ return std::stod(cmd("TEC:ITE?\r"));}
void Arroyo5305::set_ITESetPoint(float I) {
  cmd_no_response(construct_cmd("TEC:ITE ", I, "\r"));
}

float Arroyo5305::get_ITESetPoint() {
  return std::stod(cmd("TEC:SET:ITE?\r"));
}

using foxtrot::devices::TECHeatCool;
TECHeatCool Arroyo5305::get_heatcool()
{
  auto resp = cmd("TEC:HEATCOOL?\r");
  TECHeatCool out;
  switch(resp[0])
    {
    case('B'): out = TECHeatCool::BOTH; break;
    case('H'): out = TECHeatCool::HEAT; break;
    case('C'): out = TECHeatCool::COOL; break;
     
    default:
      _lg.strm(sl::error) << "response for heatcool command was: "<< resp;
      throw foxtrot::DeviceError("invalid heatcool response!");
    }

  return out;

}

void Arroyo5305::set_heatcool(TECHeatCool val)
{
  std::string s;
  switch(val)
    {
    case(TECHeatCool::BOTH): s = "BOTH"; break;
    case(TECHeatCool::HEAT): s = "HEAT"; break;
    case(TECHeatCool::COOL): s = "COOL"; break;
    default:
      throw std::logic_error("invalid heat/cool val, should never reach here");
    }

  auto cmdstr = construct_cmd("TEC:HEATCOOL ", s, "\r");
  _lg.strm(sl::trace) << "cmdstr: " << cmdstr;

  cmd_no_response(cmdstr);

}


using foxtrot::devices::TECGain;

TECGain Arroyo5305::get_gain()
{
  auto resp = cmd("TEC:GAIN?\r");
  _lg.strm(sl::trace) << "response: " << resp;
  
  if(resp.find("PID"))
    return TECGain::PID;

  short gainval = std::stoi(resp);
  return static_cast<TECGain>(gainval);
  
}

void Arroyo5305::set_gain(TECGain g)
{
  if(g == TECGain::PID)
    {
      cmd_no_response("TEC:GAIN PID\r");
      return;
    }

  int gval = static_cast<int>(g);
  cmd_no_response(construct_cmd("TEC:GAIN ", gval, "\r"));
}

float Arroyo5305::resistance()
{
  return std::stod(cmd("TEC:R?\r"));
}

std::tuple<float, unsigned short, unsigned short> Arroyo5305::get_fanspeed()
{
  auto resp = cmd("TEC:FAN?\r");

  _lg.strm(sl::debug) << "response was: " << resp;
  auto commapos = resp.find(',');
  if(commapos == std::string::npos)
    throw std::logic_error("couldn't find first comma in response string.");

  auto commapos2 = resp.find(',', commapos+1);
  if(commapos2 == std::string::npos)
    throw std::logic_error("couldn't find second comma in response string.");


  float speed;
  auto speedstr = resp.substr(0,commapos);
  if(speedstr == "OFF")
    speed = 0.0;
  else
    speed = std::stod(speedstr);

  std::string modestr = resp.substr(commapos+1,commapos2-commapos -1);
  _lg.strm(sl::trace) << "modestr:" <<modestr;
  unsigned short mode = std::stoi(modestr);

  std::string delaystr = resp.substr(commapos2+1, std::string::npos);
  _lg.strm(sl::trace) << "delaystr: " << delaystr;
  unsigned short delay = std::stoi(delaystr);

  return {speed, mode, delay};

}

void Arroyo5305::set_fanspeed(float speed, short unsigned mode, short unsigned delay)
{
  auto cmdstr = construct_cmd("TEC:FAN " , speed , "," , mode, ",", delay, "\r");
  cmd_no_response(cmdstr);
}

// void Arroyo5305::set_fanspeed(std::tuple<float, unsigned short, unsigned
// short> tplarg)
// {
//   std::apply(Arroyo5305::set_fanspeed, tplarg);
// }

unsigned char Arroyo5305::event()
{
  return std::stoi(cmd("TEC:EVE?\r"));
}


short unsigned Arroyo5305::get_outputoffregister()
{
  return std::stoi(cmd("TEC:OUTOFF?\r"));
  
}
void Arroyo5305::set_outputoffregister(short unsigned r)
{
  cmd_no_response(construct_cmd("TEC:OUTOFF ", r, "\r"));
}

using foxtrot::devices::TECSensorType;
TECSensorType Arroyo5305::get_sensortype(int idx)
{

  if(idx <1)
    throw std::out_of_range("invalid idx, must be >= 1");
  
  auto reqstr = construct_cmd("TEC:SEN? ", idx, "\r");

  auto resp = cmd(reqstr);
  _lg.strm(sl::trace) << "response is: " << resp;
  auto id  = std::stoi(resp);
  _lg.strm(sl::trace) << "id is: " << id;  
  return static_cast<TECSensorType>(id);
  
}


void Arroyo5305::set_sensortype(int idx, TECSensorType tp)
{

  if(idx <1)
    throw std::out_of_range("invalid idx, must be >= 1");
  
  auto cmdstr = construct_cmd("TEC:SEN ", static_cast<short unsigned>(tp),",", idx, "\r");
  cmd_no_response(cmdstr);
}



void Arroyo5305::strip_crlf(std::string& str)
{
  str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
}

RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::Arroyo5305;
  using namespace foxtrot::devices;

  registration::enumeration<TECSensorType>("foxtrot::devices::TECSensorType")
    ( value("DISABLED", TECSensorType::DISABLED),
      value("THERMISTOR_100uA", TECSensorType::THERMISTOR_100uA),
      value("THERMISTOR_10uA", TECSensorType::THERMISTOR_10uA),
      value("LM335", TECSensorType::LM335),
      value("AD590", TECSensorType::AD590),
      value("RTD_100", TECSensorType::RTD_100),
      value("RTD_100_4WIRE", TECSensorType::RTD_100_4WIRE),
      value("RTD_1000", TECSensorType::RTD_1000),
      value("RTD_1000_4WIRE", TECSensorType::RTD_1000_4WIRE)
      );

  
    // (value("DISABLED", TECSensorType::DISABLED),
    //  value("THERMISTOR_100uA", TECSensorType::THERMISTOR_100uA)
    //  value("THERMISTOR_10uA", TECSensorType::THERMISTOR_10uA),
    //  value("LM335", TECSensorType::LM335),
    //  value("AD590", TECSensorType::AD590),
    //  value("RTD_100", TECSensorType::RTD_100),
    //  value("RTD_1000", TECSensorType::RTD_1000),
    //  value("RTD_1000_4WIRE", TECSensorType::RTD_1000_4WIRE)
    // 	   );


  registration::enumeration<TECGain>("foxtrot::devices::TECGain")
    ( value("ONE", TECGain::ONE),
      value("THREE", TECGain::THREE),
      value("FIVE", TECGain::FIVE),
      value("TEN", TECGain::TEN),
      value("THIRTY", TECGain::THIRTY),
      value("FIFTY", TECGain::FIFTY),
      value("ONE_HUNDRED", TECGain::ONE_HUNDRED),
      value("THREE_HUNDRED", TECGain::THREE_HUNDRED),
      value("PID", TECGain::PID));
    
  registration::enumeration<TECHeatCool>("foxtrot::devices::TECHeatCool")
    ( value("BOTH", TECHeatCool::BOTH),
      value("HEAT", TECHeatCool::HEAT),
      value("COOL", TECHeatCool::COOL));

  registration::enumeration<TECControlMode>("foxtrot::devices::TECControlMode")
    ( value("ITE_MODE", TECControlMode::ITE_MODE),
      value("R_MODE", TECControlMode::R_MODE),
      value("T_MODE", TECControlMode::T_MODE));
  
  registration::class_<Arroyo5305>("foxtrot::devices::Arroyo5305")
    .property_readonly("version", &Arroyo5305::version)
    .method("identify", &Arroyo5305::identify)
    .property_readonly("serialno", &Arroyo5305::serialno)
    .property("temp_setpoint", &Arroyo5305::get_temp_setpoint,
	      &Arroyo5305::set_temp_setpoint)
    .property_readonly("temp", &Arroyo5305::temp)
    .property_readonly("total_centiseconds_uptime", &Arroyo5305::total_centiseconds_uptime)
    .method("total_centiseconds_timer", &Arroyo5305::total_centiseconds_timer)
    .property("remote_volt_sense", &Arroyo5305::get_remote_volt_sense,
	      &Arroyo5305::set_remote_volt_sense)
    .property_readonly("voltage", &Arroyo5305::voltage)
    .property("trate", &Arroyo5305::get_trate, &Arroyo5305::set_trate)
    .method("get_tolerance", &Arroyo5305::get_tolerance)
    .method("set_tolerance", &Arroyo5305::set_tolerance)
    .property_readonly("status", &Arroyo5305::get_status)
    .property_readonly("eventstatus", &Arroyo5305::get_eventstatus)
    .property_readonly("errors", &Arroyo5305::get_errors)
    .method("get_PID", &Arroyo5305::get_PID)
    .method("set_PID", &Arroyo5305::set_PID)
    .property("output", &Arroyo5305::get_output, &Arroyo5305::set_output)
    .property("TLoLimit", &Arroyo5305::get_TLoLimit, &Arroyo5305::set_TLoLimit)
    .property("THiLimit", &Arroyo5305::get_THiLimit, &Arroyo5305::set_THiLimit)
    .property("RLoLimit", &Arroyo5305::get_RLoLimit, &Arroyo5305::set_RLoLimit)
    .property("RHiLimit", &Arroyo5305::get_RHiLimit, &Arroyo5305::set_RHiLimit)
    .property("ITELimit", &Arroyo5305::get_ITELimit, &Arroyo5305::set_ITELimit)
    .property("Vlimit", &Arroyo5305::get_VLimit, &Arroyo5305::set_VLimit)
    .property_readonly("TECCurrent", &Arroyo5305::TECCurrent)
    .property("heatcool", &Arroyo5305::get_heatcool, &Arroyo5305::set_heatcool)

    .property("gain", &Arroyo5305::get_gain, &Arroyo5305::set_gain)

    .property_readonly("resistance", &Arroyo5305::resistance)
    .method("get_fanspeed", &Arroyo5305::get_fanspeed)
    .method("set_fanspeed", &Arroyo5305::set_fanspeed)
    (parameter_names("speed", "mode", "delay"))
    .property_readonly("event", &Arroyo5305::event)
    .property("outputoffregister", &Arroyo5305::get_outputoffregister,
	      &Arroyo5305::set_outputoffregister)

    .method("set_sensortype", &Arroyo5305::set_sensortype)
    (parameter_names("idx", "tp"))
    .method("get_sensortype", &Arroyo5305::get_sensortype)
    (parameter_names("idx"))
    
    ;
  

}
