#include <foxtrot/DeviceError.h>
#include <sys/types.h>
#include "APT.h"
#include "APT_defs.hh"
#include <foxtrot/ProtocolTimeoutError.h>

#ifdef linux
#include <byteswap.h>
#endif

#include <sstream>
#include <thread>
#include <chrono>
#include <string>

#include <set>
#include <cmath>

using std::cout;
using std::endl;

using namespace foxtrot::devices;

const std::set<foxtrot::devices::bsc203_opcodes> motor_finish_messages =
    {foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED, 
     foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,
     foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_STOPPED};

const std::set<foxtrot::devices::bsc203_opcodes> motor_status_messages =
    {foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED, 
     foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,
     foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_STOPPED,
     foxtrot::devices::bsc203_opcodes::MGMSG_MOT_GET_STATUSUPDATE};

#if __GNUC__ > 9
const foxtrot::parameterset bsc203_class_params
{
  {"baudrate", 115200},
  {"stopbits", 1},
  {"flowcontrol", "hardware"},
  {"parity", "none"}
};
#else
const foxtrot::parameterset bsc203_class_params
{
  {"baudrate", 115200u},
  {"stopbits", 1u},
  {"flowcontrol", "hardware"},
  {"parity", "none"}
};
#endif


using namespace foxtrot::devices;
using foxtrot::devices::APT;

foxtrot::devices::AptUpdateMessageScopeGuard::AptUpdateMessageScopeGuard(APT* obj, destination dest)
  : _obj(obj), _dest(dest), lg("AptUpdateMessageScopeGuard")
{
  if(obj != nullptr)
    obj -> start_update_messages(dest);

  lg.strm(sl::debug) << "entering update message scope guard";
};

foxtrot::devices::AptUpdateMessageScopeGuard::~AptUpdateMessageScopeGuard()
{
  if(_obj != nullptr)
    {
    _obj -> stop_update_messages(_dest);   
    lg.strm(sl::debug) << "leaving update message scope guard";
    }
  else
    {
      lg.strm(sl::debug) << "scope guard destroyed but no object left";
    }
}


foxtrot::devices::APT::APT(std::shared_ptr< foxtrot::protocols::SerialPort > proto) 
: foxtrot::Device(proto),  _lg("APT"), _serport(proto)
{
  _serport->Init(&bsc203_class_params);
  _serport->setDrain(true);
  _serport->flush();
}


foxtrot::devices::APT::~APT() {}


#pragma pack(push, 1)
struct poscounter_throwaway {
  unsigned short chan_ident;
  int position_counter_value;
};
#pragma pack(pop)

foxtrot::devices::ThorlabsMotorError::ThorlabsMotorError(const std::string& msg)
: Error(msg)
{};


void foxtrot::devices::APT::transmit_message(bsc203_opcodes opcode, unsigned char p1, unsigned char p2, destination dest, destination src)
{
  auto optpr = reinterpret_cast<unsigned char*>(&opcode);
  std::array<unsigned char, 6> header{ optpr[0], optpr[1], p1, p2, static_cast<unsigned char>(dest) ,static_cast<unsigned char>(src)};
  
  _lg.Trace("writing to serial port...");
  _serport->write(std::string(header.begin(), header.end()));


  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  
}

void foxtrot::devices::APT::transmit_message(bsc203_opcodes opcode,
                                             destination dest, motor_channel_idents chan,
                                             destination src)
{
      transmit_message(opcode,
                     static_cast<unsigned short>(chan),
                     0, dest, src);
    
}


bool foxtrot::devices::APT::check_status_bits(const allstatus& status, bool check_limitswitch, bool require_moving)
{
    unsigned int statusbits;
    bool is_dcstatus;
    std::visit([&statusbits, &is_dcstatus](auto& v) { 

        statusbits = v.statusbits;
        
        if constexpr(std::is_same_v<decltype(v), dcstatus>)
            is_dcstatus = true;
        else
            is_dcstatus = false;
        
    }, status);
    
    std::bitset<32> bits(statusbits);
    _lg.strm(sl::trace) << "statusbits: " << bits;
    
    if(check_limitswitch and (bits[0] or bits[1] or bits[2] or bits[3]))
        return false;
    
    if(require_moving and  not (bits[4] or bits[5] or bits[6] or bits[7]))
        return false;
        
    return true;
}

foxtrot::devices::bsc203_reply foxtrot::devices::APT::receive_message_sync(bsc203_opcodes expected_opcode, destination expected_source, bool* has_data, bool check_opcode, unsigned* received_opcode)
{
    bsc203_reply out;

    auto [aptreply, opcode] = receive_sync_common(expected_source);

    
    if (received_opcode != nullptr)
        *received_opcode = opcode;

    if(opcode != static_cast<decltype(opcode)>(expected_opcode))
    {
      auto errtp = check_opcode? sl::error : sl::debug;

      _lg.strm(errtp) << "got opcode: 0x" << std::hex << opcode;
      _lg.strm(errtp) << "but expected: 0x"<< std::hex <<  static_cast<decltype(opcode)>(expected_opcode);
        
      if (check_opcode)
	throw DeviceError("received unexpected opcode");
    }

    if(has_data!=nullptr)
      *has_data = aptreply.data.has_value();

    if(aptreply.data.has_value())
      out.data = std::move(*aptreply.data);
    
    out.p1 = aptreply.p1;
    out.p2 = aptreply.p2;
    
    return out;
    
}


void foxtrot::devices::APT::start_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_START_UPDATEMSGS, 0x0A,0x0, dest);

}

void foxtrot::devices::APT::stop_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_STOP_UPDATEMSGS, 0x0,0x0, dest);
}

void foxtrot::devices::APT::start_motor_messages(destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_MOT_RESUME_ENDOFMOVEMSGS, 0x0, 0x0, dest);
}

void foxtrot::devices::APT::stop_motor_messages(destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_MOT_SUSPEND_ENDOFMOVEMSGS, 0x0, 0x0, dest);
}


int foxtrot::devices::APT::get_position_counter(destination dest, motor_channel_idents channel)
{
  auto repl = request_response_struct<poscounter_throwaway>(
      bsc203_opcodes::MGMSG_MOT_REQ_POSCOUNTER,
      bsc203_opcodes::MGMSG_MOT_GET_POSCOUNTER,
      dest, 0, 0);
  
  return repl.position_counter_value;
  
}

void foxtrot::devices::APT::set_position_counter(destination dest, motor_channel_idents channel, int val)
{
  poscounter_throwaway in;
  in.position_counter_value = val;
  in.chan_ident = static_cast<decltype(in.chan_ident)>(channel);
  auto data = copy_struct_to_array(in);
  transmit_message(bsc203_opcodes::MGMSG_MOT_SET_POSCOUNTER, data, dest);
}


foxtrot::devices::homeparams foxtrot::devices::APT::get_homeparams(destination dest, motor_channel_idents channel)
{
  auto ret = request_response_struct<homeparams>(bsc203_opcodes::MGMSG_MOT_REQ_HOMEPARAMS,
						 bsc203_opcodes::MGMSG_MOT_GET_HOMEPARAMS,
						 dest, static_cast<unsigned short>(channel), 0);

  return ret;

}

void foxtrot::devices::APT::set_homeparams(destination dest,
                                           const homeparams& params)
{
    auto dat = copy_struct_to_array(params);    
    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_HOMEPARAMS, dat, dest);

}

limitswitchparams foxtrot::devices::APT::get_limitswitchparams(destination dest, motor_channel_idents channel)
{
    auto out = request_response_struct<limitswitchparams>(bsc203_opcodes::MGMSG_MOT_REQ_LIMSWITCHPARAMS, bsc203_opcodes::MGMSG_MOT_GET_LIMSWITCHPARAMS, dest, static_cast<unsigned short>(channel), 0);
    
    return out;
}

void foxtrot::devices::APT::set_limitswitchparams(destination dest,
                                                  const limitswitchparams& params)
{
    
    auto dat = copy_struct_to_array(params);
    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_LIMSWITCHPARAMS,
                     dat, dest);
}




bool foxtrot::devices::APT::get_channelenable(destination dest, motor_channel_idents channel)
{
    transmit_message(bsc203_opcodes::MGMSG_MOD_REQ_CHANENABLESTATE, dest, channel);

    auto ret = receive_message_sync_check(bsc203_opcodes::MGMSG_MOD_GET_CHANENABLESTATE, dest);
    //    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOD_GET_CHANENABLESTATE,dest);
    
    bool onoff = (ret.p2 == 0x01) ? true : false;
    
    return onoff;
}

void foxtrot::devices::APT::set_channelenable(destination dest, motor_channel_idents channel, bool onoff)
{
    unsigned char enable_disable = onoff? 0x01 : 0x02;

    transmit_message(bsc203_opcodes::MGMSG_MOD_SET_CHANENABLESTATE,static_cast<unsigned char>(channel),enable_disable, dest);
    
}


hwinfo foxtrot::devices::APT::get_hwinfo(destination dest,
							   std::optional<destination> expd_src)
{   
    auto out = request_response_struct<hwinfo>(bsc203_opcodes::MGMSG_MOD_REQ_HWINFO,
                                               bsc203_opcodes::MGMSG_MOD_GET_HWINFO, dest,0x00, 0x00,
					       expd_src);

    return out;
};


std::variant<channel_status, dcstatus> foxtrot::devices::APT::get_status(destination dest, motor_channel_idents channel)
{
    auto repl = request_response_struct<dcstatus>(_traits.status_request_code, _traits.status_get_code, dest, static_cast<unsigned char>(channel),
                                                  0, dest);
    
    return repl;
    
}


void foxtrot::devices::APT::start_absolute_move(destination dest, motor_channel_idents chan, unsigned int target)
{
    auto msgdata = get_move_request_header_data(target, chan);
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_ABSOLUTE, msgdata, dest);
}


void foxtrot::devices::APT::start_relative_move(destination dest, motor_channel_idents chan, int movedist)
{
    auto msgdata = get_move_request_header_data(movedist, chan);
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_RELATIVE, msgdata, dest);
}
                                        
void foxtrot::devices::APT::stop_move(destination dest, motor_channel_idents channel, bool immediate)
{
    unsigned short stopmode = immediate? 0x01 : 0x02;
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_STOP, static_cast<unsigned short>( channel), stopmode, dest);
}


void foxtrot::devices::APT::absolute_move_blocking(destination dest, motor_channel_idents channel, unsigned int target)
{
  
  auto tm = estimate_abs_move_time(dest, channel, target);
  _lg.strm(sl::debug) << "estimated move time(ms): "<< tm.count() ;


  auto limstate = is_limited(dest, channel);
  
  start_absolute_move(dest, channel, target);
  
  wait_blocking_move(_traits.status_get_code,
                     _traits.complete_success_code,
                     dest, channel, std::chrono::milliseconds(1000),
                     tm, limstate);
  
}

bool foxtrot::devices::APT::is_limited(destination dest, motor_channel_idents channel)
{
  _lg.strm(sl::trace) << "is_limited";
  auto stat = get_status(dest, channel);
  _lg.strm(sl::trace) << "got status";
  bool out = std::visit([] (auto& v) {
    auto statusbits = v.statusbits;
    std::bitset<32> bits(statusbits);
    return bits[0] or bits[1] or bits[2] or bits[3];}, stat);

  return out;
}


void foxtrot::devices::APT::wait_blocking_move(bsc203_opcodes statusupdateopcode,
                                               bsc203_opcodes completionexpectedopcode,
                                               destination dest, motor_channel_idents chan,
                                               std::chrono::milliseconds update_timeout,
                                               std::chrono::milliseconds total_move_timeout, bool init_limit_state
                                              )
{
  AptUpdateMessageScopeGuard msgguard(this, dest);
  
  
    _lg.strm(sl::debug) << "waiting for motion complete message";

    auto starttime = std::chrono::steady_clock::now();
    bool should_finish = false;


    int fail_status_retries = 0;
    
    while(true)
    {
        auto [repl, opcode] = receive_message_sync_check(_traits.motor_status_messages.begin(), 
                                                         _traits.motor_status_messages.end(), dest, update_timeout);
        
        if(opcode == static_cast<decltype(opcode)>(statusupdateopcode))
        {
            //NOTE: this should always have a data field, no need to check unless
            // we see things going really wrong
            _lg.strm(sl::trace) << "received motor status update msg";
            auto dat = *repl.data;
            
            //NOTE: this is the thing that'll need to be changed probably for BSC203
            dcstatus stat;
            std::copy(dat.begin(), dat.end(), reinterpret_cast<unsigned char*>(&stat));
	    auto thischeck = check_status_bits(stat, true, true);
            if(!thischeck)
	      {
		//check if the motor thinks it's homing
		std::bitset<32> bits(stat.statusbits);
		if(bits[9])
		  {
		    _lg.strm(sl::info) << "motor status failed, but reports still homing";
		    //NOTE: this will happen both when a motor is homing and hits a limit briefly, but ALSO when it has physically crashed into something. Therefore, check this a few times and if it gets above a threshold bail out, sending an emergency stop message...
		    //For now we make it not configurable, if we need that later we'll add it
		    if(fail_status_retries++ >= _traits.n_ignore_failed_motor_msgs)
		      {
			_lg.strm(sl::error) << "motor status checks failed " << fail_status_retries << " times, likely motor has stalled or crashed!";
		    
			stop_move(dest, chan, true);
			throw DeviceError("motor status check failed. Motor crash suspected. Emergency stop issued. If this is not a crash, maybe retry number needs adjusting!");
		      }
		    

		    
		    continue;
		  }
		else if( (bits[0] or bits[1] or bits[2] or bits[3]) and init_limit_state)
		  {

		    _lg.strm(sl::debug) << "init_limit_state: " << (int) init_limit_state;
		    _lg.strm(sl::info) << "got a limit switch message, but we started there so ignoring one";
		    init_limit_state = false;
		    continue;

		  }

		_serport->flush();
		throw ThorlabsMotorError("motor status check failed! (Maybe a limit switch has been engaged)");

	      }
	    
        }
        else
        {
            _lg.strm(sl::debug) << "received move completion message!";
            if(opcode != static_cast<decltype(opcode)>(completionexpectedopcode))
                throw ThorlabsMotorError("got unexpected type of completion message!");
            break;
            
        }
    }
    _serport->flush();
}


void foxtrot::devices::APT::home_move_blocking(destination dest, motor_channel_idents channel)
{

  auto limitstate = is_limited(dest, channel);

  auto stat = get_status(dest, channel);
  bool is_homing_already = extract_status_bit<32>(stat, static_cast<unsigned char>(status_bit_indices::is_homing));
  
  if(is_homing_already)
    _lg.strm(sl::info) << "the stage is already homing, FYI...";

  _lg.strm(sl::debug) << "cancelling all running moves...";
  stop_move(dest, channel, true);

  stat = get_status(dest, channel);
  is_homing_already = extract_status_bit<32>(stat, static_cast<unsigned char>(status_bit_indices::is_homing));

  if(is_homing_already)
    {
      _lg.strm(sl::error) << "it's still homing, somehow stop didn't work...";
      throw foxtrot::DeviceError("failed to stop a homing move before homing...");
    }


  
  start_home_channel(dest, channel);
 

 
  wait_blocking_move(_traits.status_get_code,
                     bsc203_opcodes::MGMSG_MOT_MOVE_HOMED, 
                     dest, channel, std::chrono::milliseconds(2000), 
                     //WARNING: this one is not used for now 
                     std::chrono::milliseconds(1000), limitstate);

}


void foxtrot::devices::APT::relative_move_blocking(destination dest, motor_channel_idents channel, int target)
{
    
    auto tm = estimate_rel_move_time(dest, channel, target);
    _lg.strm(sl::debug) << "estimated move time(ms): "<< tm.count() ;


    auto limitstate = is_limited(dest, channel);
    start_relative_move(dest, channel, target);
    wait_blocking_move(_traits.status_get_code,
                       bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED,
                       dest, channel, std::chrono::milliseconds(1000),
                       tm, limitstate);
    
    
}



foxtrot::devices::velocity_params foxtrot::devices::APT::get_velocity_params(foxtrot::devices::destination dest, motor_channel_idents channel)
{

  auto out =request_response_struct<velocity_params>(bsc203_opcodes::MGMSG_MOT_REQ_VELPARAMS, bsc203_opcodes::MGMSG_MOT_GET_VELPARAMS, dest, static_cast<unsigned short>(channel),0x0);

    return out;

}

void foxtrot::devices::APT::set_velocity_params (foxtrot::devices::destination dest, const foxtrot::devices::velocity_params& velpar)
{

  auto data = copy_struct_to_array(velpar);  
  transmit_message(bsc203_opcodes::MGMSG_MOT_SET_VELPARAMS, data, dest);

}





void foxtrot::devices::APT::start_home_channel(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan)
{
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_HOME,static_cast<unsigned char>(chan),0,dest);
};



std::chrono::milliseconds foxtrot::devices::APT::estimate_abs_move_time(destination dest, motor_channel_idents channel, unsigned int target, std::optional<unsigned int> start)
{
  int tgt_dist;
  if(!start.has_value())
    {
      auto statvar = get_status(dest, channel);
      dcstatus stat = std::get<dcstatus>(statvar);
      
      tgt_dist = std::abs((int)target - (int)stat.position);
    }
  else
    {
      tgt_dist = std::abs((int)target - (int)(*start));
    }
  
  return estimate_rel_move_time(dest, channel, tgt_dist);
}

std::chrono::milliseconds foxtrot::devices::APT::estimate_rel_move_time(destination dest, motor_channel_idents channel, int target)
{
  int tgt_dist = std::abs(target);

  _lg.strm(sl::debug) << "retrieving velocity params to calculate move time...";

  auto velparams = get_velocity_params(dest, channel);

  //this cast is to prevent a core dump floating point exception
  //when it happens to be 0
  auto accel = static_cast<double>(velparams.acceleration);
  
  //t1 is the "triangle" area of the vel/ time graph
  //NOTE: the over 16 I think is to do with the trinamics/non-trinamics thing
  double t1  = velparams.maxvel / accel / 16;
  double dist1 = velparams.acceleration * t1 * t1;

  
  _lg.strm(sl::debug) << "target distance for move: " << tgt_dist ;

  //in this case the acceleration periods will be truncated. To be safe, estimate
  //it'll take the full time anyway
  if(tgt_dist < (2*dist1))
    {
      _lg.strm(sl::trace) << "move distance will truncate edges of target";
      double t3 = std::sqrt((double)tgt_dist / accel);
      _lg.strm(sl::trace) << "t3: " << t3;
      unsigned ms = std::ceil(t3*1000);
      return std::chrono::milliseconds(ms);
      
      
    }
 
  double remdist = tgt_dist - 2*dist1;
  double remtime = remdist / velparams.acceleration;

  _lg.strm(sl::debug) << "extra time: " << remtime;

  unsigned ms = std::ceil(remtime * 1000 );
  return std::chrono::milliseconds(ms);  

}

foxtrot::devices::apt_reply foxtrot::devices::APT::receive_message_sync_check(bsc203_opcodes expected_opcode, destination expected_source, optional<milliseconds> timeout, bool discard_motor_status)
{

  
  auto [out, recvd_opcode ] = receive_sync_common(expected_source, timeout);


  if(recvd_opcode != static_cast<unsigned short>(expected_opcode))
    {

      if(discard_motor_status)
	{
	  auto statpos = std::find_if(motor_status_messages.begin(),
				      motor_status_messages.end(), [recvd_opcode](auto& c) { return static_cast<unsigned short>(c) == recvd_opcode;});

	  if(statpos != motor_status_messages.end())
	    {
	      _lg.strm(sl::warning) << "got an extra motor status, but we have been told to ignore it...";
	      _lg.strm(sl::debug) << "extra motor status has opcode: " << std::hex << static_cast<unsigned short>(recvd_opcode) << std::dec;

	      return receive_message_sync_check(expected_opcode, expected_source, timeout, false);
	    }

	}
      
      _lg.strm(sl::error) << "expected opcode:  0x" << std::hex << static_cast<unsigned short>(expected_opcode);
      _lg.strm(sl::error) << "but received : 0x" << std::hex << recvd_opcode;
      throw DeviceError("unexpected opcode received in message!");
      
    }
  
  return out;
}


void foxtrot::devices::APT::attempt_error_recover()
{
  _lg.strm(sl::info) << "attempting error recovery...";
  _lg.strm(sl::info) << "flusing serial port...";
  _serport->flush();


};



std::tuple<foxtrot::devices::apt_reply, unsigned short> foxtrot::devices::APT::receive_sync_common(destination expected_source, optional<milliseconds> timeout, bool throw_on_errors)
{

  apt_reply out;

  //read the header in from the serial port
  if(!timeout.has_value())
      timeout = _serport->calc_minimum_transfer_time(6) * 5;


  _lg.strm(sl::trace) << "timeout is: " << (*timeout).count();
  auto headerstr = _serport->read_definite(6, *timeout);

  unsigned short recv_opcode = ( static_cast<unsigned>(headerstr[1]) <<8 ) | static_cast<unsigned>(headerstr[0] & 0xFF);

  
  if (headerstr[4] == 0)
    {
      _lg.strm(sl::error) << "headerstr is (hex): " << std::hex << headerstr << std::dec;            _lg.Error("null destination");
      _lg.strm(sl::error) << "header str is of length: " << headerstr.size();
      _lg.Info("flushing serial port");
      _serport->flush();
        throw DeviceError("received null destination");
    }


  if( (recv_opcode == static_cast<unsigned short>(bsc203_opcodes::MGMSG_HW_RESPONSE)) and throw_on_errors) 
    {
      _lg.strm(sl::error) << "got MGMSG_HW_RESPONSE message,usually an error!";


    }

  if(std::all_of(headerstr.begin(), headerstr.end(), [](auto& c) { return c == 0;}))
    {
      _lg.strm(sl::error) << "headerstr is ALL ZEROS!";
      _lg.strm(sl::info) << "flushing serial port" ;
      _serport->flush();
      throw DeviceError("received all zeros");
    }

  auto src = headerstr[5];
    if(src != static_cast<decltype(src)>(expected_source))
    {
      _lg.strm(sl::error) << "headerstr is: " << headerstr;
      std::ostringstream oss;
      oss << std::hex;
      for(auto c : headerstr)
	oss <<  static_cast<unsigned int>(c) << ",";
      _lg.strm(sl::error) << "header str (hex) is: " << oss.str();	    
      _lg.strm(sl::error) << "header str is of length: " << headerstr.size();

      _lg.Info("flushing serial port");
      _serport->flush();
      _lg.Error("unexpected source: " + std::to_string(src) + ", expected: " +  std::to_string(static_cast<decltype(src)>(expected_source)));
        throw DeviceError("received unexpected source");

    }


    //check if data packet present 
    if( (headerstr[4] & 0x80) == 0x80)
    {
        //WARNING: IS THE DLEN MSB OR LSB?
      _lg.strm(sl::trace) << "headerstr[3]: " << (int) headerstr[3] << " headerstr[2]: " << (int) headerstr[2];
      unsigned short dlen = (static_cast<unsigned short>(headerstr[3]) << 8) | static_cast<unsigned short>(headerstr[2]);  
      
      _lg.Trace("data packet present, length: " + std::to_string(dlen));

      auto datatimeout = _serport->calc_minimum_transfer_time(dlen);
      _lg.strm(sl::trace) << "data transfer exp'd transfer time:" << datatimeout.count();
      auto data = _serport->read_definite(dlen, std::chrono::seconds(1));
      
      out.data = std::vector<unsigned char>(data.begin(),data.end());

    }
    
    out.p1 = headerstr[2];
    out.p2 = headerstr[3];
    
    return {out, recv_opcode};

}



RTTR_REGISTRATION{
    using namespace rttr;
    using foxtrot::devices::APT;
    registration::class_<APT>("foxtrot::devices::APT")

      
    .method("get_channelenable", &APT::get_channelenable)
    (parameter_names("dest", "channel"))
      .method("set_channelenable", &APT::set_channelenable)
      (parameter_names("dest", "channel", "onoff"))
      .method("get_hwinfo", &APT::get_hwinfo)
      (parameter_names("dest", "expd_src"))
      .method("get_status", &APT::get_status)
      (parameter_names("dest", "channel"))
      .method("stop_move", &APT::stop_move)
      (parameter_names("dest", "channel", "immediate"))
      .method("absolute_move_blocking", &APT::absolute_move_blocking)
      (parameter_names("dest", "channel", "target"))
      .method("relative_move_blocking", &APT::relative_move_blocking)
      (parameter_names("dest", "channel", "target"))
      .method("home_move_blocking", &APT::home_move_blocking)
      (parameter_names("dest","channel"))
    .method("set_velocity_params", &APT::set_velocity_params)
    (parameter_names("destination", "velocity parameters"))
    .method("get_velocity_params", &APT::get_velocity_params)
      (parameter_names("destination", "channel"))
      .method("get_position_counter", &APT::get_position_counter)
              (parameter_names("dest", "channel"))
     .method("set_position_counter", &APT::set_position_counter)
             (parameter_names("dest", "channel", "val"))
      .method("get_homeparams", &APT::get_homeparams)
      (parameter_names("dest", "channel"))
      .method("set_homeparams", &APT::set_homeparams)
      (parameter_names("dest", "params"))
      .method("get_limitswitchparams", &APT::get_limitswitchparams)
      (parameter_names("dest", "params"))
      .method("set_limitswitchparams", &APT::set_limitswitchparams)
      (parameter_names("dest", "params"))
      .method("attempt_error_recover", &APT::attempt_error_recover)
      
      
      ;

//     .method("home_channel", &APT::home_channel)
//     (parameter_names("destination", "channel"));
//     
    
    //Custom structs
    using foxtrot::devices::bsc203_reply;
    registration::class_<bsc203_reply>("foxtrot::devices::bsc203_reply")
    .constructor()(policy::ctor::as_object)
    .property("p1", &bsc203_reply::p1)
    .property("p2", &bsc203_reply::p2)
    .property("data", &bsc203_reply::data);
    
    using foxtrot::devices::hwinfo;
    registration::class_<hwinfo>("foxtrot::devices::hwinfo")
    .constructor()(policy::ctor::as_object)
    .property("serno", &hwinfo::serno)
    .property("modelno", &hwinfo::modelno)
    .property("type", &hwinfo::type)
    .property("fwvers", &hwinfo::fwvers)
    .property("notes", &hwinfo::notes)
    .property("emptyspace", &hwinfo::emptyspace)
    .property("HWvers", &hwinfo::HWvers)
    .property("modstate", &hwinfo::modstate)
    .property("nchans", &hwinfo::nchans);
    
    using foxtrot::devices::channel_status;
    registration::class_<channel_status>("foxtrot::devices::channel_status")
    .constructor()(policy::ctor::as_object)
    .property("chan_indent", &channel_status::chan_ident)
    .property("position", &channel_status::position)
    .property("enccount", &channel_status::enccount)
    .property("statusbits", &channel_status::statusbits);
    
    using foxtrot::devices::velocity_params;
    registration::class_<velocity_params>("foxtrot::devices::velocity_params")
    .constructor()(policy::ctor::as_object)
    .property("chan_indent", &velocity_params::chan_ident)
    .property("minvel", &velocity_params::minvel)
    .property("acceleration", &velocity_params::acceleration)
    .property("maxvel", &velocity_params::maxvel);
    
    using foxtrot::devices::homeparams;
    registration::class_<homeparams>("foxtrot::devices::homeparams")
    .constructor()(policy::ctor::as_object)
    .property("chan_ident", &homeparams::chan_ident)
    .property("homeDir", &homeparams::homeDir)
    .property("limitSwitch", &homeparams::limitSwitch)
    .property("homeVelocity", &homeparams::homeVelocity)
    .property("offsetDistance", &homeparams::offsetDistance);
    
    using foxtrot::devices::limitswitchparams;
    registration::class_<limitswitchparams>("foxtrot::devices::limitswitchparams")
    .constructor()(policy::ctor::as_object)
    .property("chan_ident", &limitswitchparams::chan_ident)
    .property("CWhard", &limitswitchparams::CWhard)
    .property("CCWhard", &limitswitchparams::CCWhard)
    .property("CWsoft", &limitswitchparams::CWsoft)
    .property("CCWsoft", &limitswitchparams::CCWsoft)
    .property("limitMode", &limitswitchparams::limitMode);

    using foxtrot::devices::dcstatus;
    registration::class_<dcstatus>("foxtrot::devices::dcstatus")
      .constructor()(policy::ctor::as_object)
      .property("chan_ident", &dcstatus::chan_ident)
      .property("position", &dcstatus::position)
      .property("velocity", &dcstatus::velocity)
      .property("motorcurrent", &dcstatus::motorcurrent)
      .property("statusbits", &dcstatus::statusbits);
      
    
    //Custom enums
    using foxtrot::devices::destination;
    registration::enumeration<destination>("foxtrot::devices::destination")
    (value("host", destination::host),
     value("rack", destination::rack),
     value("genericUSB", destination::genericUSB),
     value("bay1", destination::bay1),
     value("bay2", destination::bay2),
     value("bay3", destination::bay3));
    
    using foxtrot::devices::motor_channel_idents;
    registration::enumeration<motor_channel_idents>("foxtrot::devices::motor_channel_idents")
    (value("channel_1", motor_channel_idents::channel_1),
     value("channel_2", motor_channel_idents::channel_2),
     value("channel_3", motor_channel_idents::channel_3),
     value("channel_4", motor_channel_idents::channel_4));
}



