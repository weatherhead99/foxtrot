#include "ssc_uc160.hh"
#include <foxtrot/protocols/SerialPort.h>
#include <chrono>
#include <optional>
#include <thread>
#include <sstream>

using namespace foxtrot::devices;
using foxtrot::protocols::SerialPort;
using std::static_pointer_cast;


const foxtrot::parameterset UC170_serial_params
  {
    {"baudrate", 9600},
    {"stopbits", 1},
    {"bits", 8},
    {"parity", "none"},
    {"flowcontrol", "none"}

  };


namespace foxtrot{
  namespace devices {

    struct UC170Impl
    {
      using TClock = std::chrono::steady_clock;
      using TTime = std::chrono::time_point<TClock>;
      
      std::optional<TTime> last_command_sent = std::nullopt;
     

      
    };


    
    using foxtrot::protocols::SerialPort;
    UC170Chiller::UC170Chiller(shared_ptr<SerialPort> proto)
      : CmdDevice(proto), _lg("UC170Chiller"), _impl(new UC170Impl)

    {
      _lg.Debug("setting up serial port with known parameters");
      proto->Init(&UC170_serial_params);
      _lg.Debug("serial port init done...");
      static_pointer_cast<SerialPort>(proto)->flush();
  
    }

    UC170Chiller::~UC170Chiller()
    {
    };
    
    string UC170Chiller::cmd(const string &request)
    {

      if(_impl->last_command_sent.has_value())
	{
      
	  _lg.strm(sl::debug) << "checking whether we can send a command... (two commands a second at most";
	  auto now =  UC170Impl::TClock::now();
	  if( *(_impl->last_command_sent) + std::chrono::milliseconds(500) < now)
	    {
	      _lg.strm(sl::debug) << "need to wait before sending command...";
	      std::this_thread::sleep_until(*(_impl->last_command_sent) + std::chrono::milliseconds(500));
	      _lg.strm(sl::debug) << "sleep over, let's get to work...";
	    }
	}

      string out;

      auto serproto = static_pointer_cast<SerialPort>(_proto);


      if(request.size() ==0)
	throw std::logic_error("can't use an empty request!");

      auto validate_and_dispatch = [&serproto, &request, this] (int reqlen)
      { if(request.size() != reqlen)
	  {
	      _lg.strm(sl::error) << "command is: " << request.size() << "bytes long";
	      throw std::logic_error("command needs 4 bytes!");
	  }
	serproto->write(request);
      };


      auto validate_response_echo = [&serproto, &request, this] (int n_data_bytes) -> std::string {
	auto timeout = serproto->calc_minimum_transfer_time(request.size());

	_lg.strm(sl::debug) << "timeout for transfer is: " << timeout.count() * 10 << " ms";
	auto rawresp = serproto->read_definite(n_data_bytes + 1, timeout * 10);
	if(rawresp[0] != request[0])
	  {
	    _lg.strm(sl::error) << "expected echo byte: " << std::bitset<8>(request[0]) << " but received back: " << std::bitset<8>(rawresp[0]);
	    throw std::runtime_error("invalid echo byte received");
	  }

	if(n_data_bytes == 0)
	  return "";
	return rawresp.substr(1);
      };
      
	std::string resp;

	unsigned char cmdbits = request[0] & 0b11111;
	
	switch(cmdbits)
	{
	case(static_cast<unsigned char>(UC170_command_sigils::SET_POINT_TEMP)):
	  validate_and_dispatch(3);
	  //TODO: needs logic for set and read
	  if( (request[0] && (0b10000)) >> 4)
	    {
	      //setting value
	      _lg.Debug("command is to set the Temperature set point");
	      resp = validate_response_echo(0);
	    }
	  else
	    {
	      //reading back value
	      _lg.Debug("command is to read back the temperature set point");
	      resp = validate_response_echo(2);
	    }
	    
	      
	  
	  break;

	case(static_cast<unsigned char>(UC170_command_sigils::ACT_TEMP)):
	  _lg.Debug("command is to read back the actual temperature");
	  validate_and_dispatch(1);
	  resp = validate_response_echo(2);
	  break;
	  
	case(static_cast<unsigned char>(UC170_command_sigils::CHECK_FAULTS)):
	  _lg.Debug("command is to check the fault status of the chiller");
	  validate_and_dispatch(1);
	  resp = validate_response_echo(1);
	  break;
	  
	case(static_cast<unsigned char>(UC170_command_sigils::PERCENT_POWER)):
	  _lg.Debug("command is to read back the % power of the chiller");
	  validate_and_dispatch(1);
	  resp = validate_response_echo(3);
	  break;

	case(static_cast<unsigned char>(UC170_command_sigils::RESET_ALARMS_RESTART)):
	  _lg.Debug("command is to reset the alarms and restart");
	  validate_and_dispatch(1);
	  resp = validate_response_echo(0);
	  break;

	default:
	  _lg.strm(sl::error) << "command byte received was: " << std::bitset<8>(request[1]) <<  std::dec;
	  throw std::logic_error("invalid command byte sent to cmd(), see logs");

	}

    
      _impl->last_command_sent = UC170Impl::TClock::now();
      return resp;
         
    }

    string UC170Chiller::prepare_command(UC170_command_sigils param, bool set)
    {
      std::bitset<8> outbyte{0xc0};
      outbyte.set(5, set);
      _lg.strm(sl::trace) << "outbyte init is: " << std::bitset<8>(outbyte);

      std::bitset<8> paramnibble(static_cast<unsigned char>(param));
    _lg.strm(sl::trace) << "paramnibble is: " << paramnibble;
    
      outbyte |= paramnibble;
    _lg.strm(sl::trace) << "full command byte is:" << outbyte;
      std::ostringstream oss;
      oss << static_cast<unsigned char>(outbyte.to_ulong());

      _lg.strm(sl::trace) << "oss contents is: " << std::bitset<8>(oss.str()[0]);
      return oss.str();

    }

    
    float UC170Chiller::get_ActualTemp()
    {
      auto cmdstr = prepare_command(UC170_command_sigils::ACT_TEMP);
      auto resp = cmd(cmdstr);

      unsigned tempdata = resp[0] | (resp[1] << 8);
      return 0.1 * tempdata;

    }

    void UC170Chiller::set_SetPointTemp(float value)
    {
      auto cmdstr = prepare_command(UC170_command_sigils::SET_POINT_TEMP, true);

      unsigned short tempint = value / 0.1;

      unsigned char templowbyte = tempint && 0xFF;
      unsigned char temphighbyte = (tempint >> 8) & 0xFF;

      std::ostringstream oss;
      oss << cmdstr << templowbyte << temphighbyte;
      cmd(oss.str());
      

    }

    float UC170Chiller::get_SetPointTemp()
    {
      auto cmdstr = prepare_command(UC170_command_sigils::SET_POINT_TEMP);
      auto resp = cmd(cmdstr);

      unsigned tempdata = resp[0] | (resp[1] << 8);
      return 0.1 * tempdata;
     
    }

    float UC170Chiller::get_OutputPower()
    {
      auto cmdstr = prepare_command(UC170_command_sigils::PERCENT_POWER);
      auto resp = cmd(cmdstr);

      bool posneg = resp[0] & 0x80;
      float percentint = (61787 - (resp[1] + 256*resp[2]))* 100 / 1235;

      return posneg ? -percentint : percentint;
      
    }

    unsigned char UC170Chiller::get_Status()
    {
      auto cmdstr = prepare_command(UC170_command_sigils::CHECK_FAULTS);
      _lg.strm(sl::trace) << "cmdstr first byte is:" << std::bitset<8>(cmdstr[0]);
      _lg.strm(sl::trace) << "cmdstr size is: " << cmdstr.size();
      auto resp = cmd(cmdstr);
      return resp[0];
    }

    void  UC170Chiller::reset_Errors()
    {
      auto cmdstr = prepare_command(UC170_command_sigils::RESET_ALARMS_RESTART);
      cmd(cmdstr);

    }

    
    
   
  }
}


RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::UC170Chiller;

  registration::class_<UC170Chiller>("foxtrot::devices::UC170Chiller")
    .property_readonly("ActualTemp", &UC170Chiller::get_ActualTemp)
    .property("SetPointTemp", &UC170Chiller::get_SetPointTemp, &UC170Chiller::set_SetPointTemp)
    .property_readonly("OutputPower", &UC170Chiller::get_OutputPower)
    .property_readonly("Status", &UC170Chiller::get_Status)
    .method("reset", &UC170Chiller::reset_Errors);

  


}
