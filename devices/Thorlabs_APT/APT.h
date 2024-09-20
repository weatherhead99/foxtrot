#pragma once

#include <vector>
#include <array>

#include "APT_defs.hh"

#include <rttr/registration>

#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Device.h>
#include <foxtrot/Error.h>

#include <optional>
#include <chrono>
#include <tuple>
#include <exception>
#include <variant>
#include <set>

//TODO: handle error conditions somehow!!!!

using std::cout;
using std::endl;

using std::set;


namespace foxtrot {
  namespace devices {



    using allstatus = std::variant<channel_status, dcstatus>;

    class ThorlabsMotorError : public Error {
    public:
	ThorlabsMotorError(const std::string& msg);
    };

    class APT;
    struct AptUpdateMessageScopeGuard
    {
      AptUpdateMessageScopeGuard(APT* obj, destination dest);
      ~AptUpdateMessageScopeGuard();

      APT* _obj = nullptr;
      destination _dest;
      foxtrot::Logging lg;
    };


    struct APTDeviceTraits
    {
      bsc203_opcodes status_request_code = bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE;
      bsc203_opcodes status_get_code = bsc203_opcodes::MGMSG_MOT_GET_STATUSUPDATE;
      bsc203_opcodes complete_success_code = bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED;

      set<bsc203_opcodes> motor_status_messages = {
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED,
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_STOPPED,
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_GET_STATUSUPDATE};

      set<bsc203_opcodes> motor_finish_messages = {
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED,
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,
	foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_STOPPED};

      int n_ignore_failed_motor_msgs = 10;

    };


    class APT : public Device
    {
      friend class AptUpdateMessageScopeGuard;
    RTTR_ENABLE(Device)

    public:

    virtual ~APT();

    bool get_channelenable(destination dest, motor_channel_idents channel);
    virtual void set_channelenable(destination dest, motor_channel_idents channel, bool onoff);

    hwinfo get_hwinfo(destination dest, std::optional<destination> expd_src=std::nullopt);

    virtual allstatus get_status(destination dest, motor_channel_idents channel);


    void stop_move(destination dest, motor_channel_idents channel, bool immediate);

    void absolute_move_blocking(destination dest, motor_channel_idents channel, unsigned int target);

    void relative_move_blocking(destination dest, motor_channel_idents channel, int target);

      void home_move_blocking(destination dest, motor_channel_idents channel);

    void set_velocity_params (destination dest, const velocity_params& velpar);
      velocity_params get_velocity_params(destination dest, motor_channel_idents channel);


      std::chrono::milliseconds estimate_abs_move_time(destination dest, motor_channel_idents channel, unsigned int target, std::optional<unsigned int> start=std::nullopt);

      std::chrono::milliseconds estimate_rel_move_time(destination dest, motor_channel_idents channel, int target);

      int get_position_counter(destination dest, motor_channel_idents channel);
      void set_position_counter(destination dest, motor_channel_idents channel, int val);

      homeparams get_homeparams(destination dest, motor_channel_idents channel);
      void set_homeparams(destination dest, const homeparams& params);

      limitswitchparams get_limitswitchparams(destination dest, motor_channel_idents channel);
      void set_limitswitchparams(destination dest, const limitswitchparams& params);

      void attempt_error_recover();

    protected:
      APT(std::shared_ptr< protocols::SerialPort > proto);



      //HACK: children should directly set this traits struct in their constructors
      APTDeviceTraits _traits = APTDeviceTraits();


      void transmit_message(bsc203_opcodes opcode, unsigned char p1, unsigned char p2, destination dest, destination src = destination::host);

      void transmit_message(bsc203_opcodes opcode, destination dest,
			    motor_channel_idents chan, destination src=destination::host);

      template<typename arrtp>
      void transmit_message(bsc203_opcodes opcode, arrtp& data, destination dest, destination src = destination::host);



      [[deprecated]]
      bsc203_reply receive_message_sync(bsc203_opcodes expected_opcode, destination expected_source,
	  bool* has_data = nullptr, bool check_opcode = true, unsigned* received_opcode = nullptr);

      apt_reply receive_message_sync_check(bsc203_opcodes expected_opcode, destination expected_source, optional<milliseconds> timeout=std::nullopt, bool discard_motor_status=true);

      template<typename It>
      std::tuple<apt_reply, bsc203_opcodes>  receive_message_sync_check(It&& allowed_opcodes_begin, It&& allowed_opcodes_end, destination expected_source, optional<milliseconds> timeout=std::nullopt);

      void wait_blocking_move(bsc203_opcodes statusupdateopcode,
			      bsc203_opcodes completionexpectedopcode,
			      destination dest, motor_channel_idents chan,
			      std::chrono::milliseconds update_timeout,
			      std::chrono::milliseconds total_move_timeout, bool init_limit_state
			     );

      bool check_status_bits(const allstatus& status, bool check_limitswitch=true,
	  bool require_moving=true);

      void start_absolute_move(destination dest, motor_channel_idents channel, unsigned int target);

      void start_relative_move(destination dest, motor_channel_idents channel, int movedist);
      void start_home_channel(destination dest, motor_channel_idents channel);

      virtual bool is_limited(destination dest, motor_channel_idents channel);


      void start_update_messages(destination dest);
      void stop_update_messages(destination dest);

      void start_motor_messages(destination dest);
      void stop_motor_messages(destination dest);



      template<typename T>
      T _response_struct_common(foxtrot::devices::bsc203_opcodes opcode_recv,
				foxtrot::devices::destination dest,
				std::optional<foxtrot::devices::destination> expd_src);

      template<typename T>
      T request_response_struct(bsc203_opcodes opcode_send, bsc203_opcodes opcode_recv, destination dest, unsigned char p1, unsigned char p2, std::optional<destination> expd_src = std::nullopt);

      template<typename T, typename arrtp>
      T request_response_struct(bsc203_opcodes opcode_send, bsc203_opcodes opcode_recv, destination dest, arrtp& data,
	  std::optional<destination> expd_src = std::nullopt
    );

      foxtrot::Logging _lg;

      std::shared_ptr<protocols::SerialPort> _serport;

    private:
      std::tuple<apt_reply, unsigned short> receive_sync_common(destination expected_source, optional<milliseconds> timeout=std::nullopt, bool throw_on_errors=true);

      template<int Size>
      bool extract_status_bit(const allstatus& stat, int bit)
      {
	return std::visit([bit] (auto& v)
	{
	  auto statusbits = v.statusbits;
	  std::bitset<Size> bits(statusbits);
	  return bits[bit];
	}, stat);

      }


    };


    template<typename T>
    std::array<unsigned char, 6> get_move_request_header_data(T distance, foxtrot::devices::motor_channel_idents chan);

  }//namespace devices
} //namespace foxtrot





template<typename arrtp>
void foxtrot::devices::APT::transmit_message(foxtrot::devices::bsc203_opcodes opcode, arrtp& data, destination  dest, destination src)
{

  auto size = data.size();
  unsigned char* len = reinterpret_cast<unsigned char*>(&size);
  unsigned char* optpr = reinterpret_cast<unsigned char*>(&opcode);
  unsigned char destaddr = static_cast<unsigned char>(dest) | 0x80;
  unsigned char srcaddr = static_cast<unsigned char>(src);

  std::array<unsigned char, 6> header{optpr[0], optpr[1], len[0],len[1], destaddr, srcaddr};

  _serport->write(std::string(header.begin(), header.end()));
  _serport->write(std::string(data.begin(), data.end()));

}


template<typename T>
T foxtrot::devices::APT::_response_struct_common(bsc203_opcodes opcode_recv,
    destination dest, std::optional<destination> expd_src)
{
    T out;
    bool has_data;
    if(!expd_src.has_value())
	expd_src = dest;

    auto ret = this->receive_message_sync_check(opcode_recv, *expd_src);

    if(!ret.data.has_value())
	throw foxtrot::DeviceError("expected struct data in response but didn't get any!");

    if(ret.data->size() != sizeof(T))
	throw std::logic_error("mismatch between received data size and struct size!");

    std::copy(ret.data->begin(), ret.data->end(), reinterpret_cast<unsigned char*>(&out));
    return out;

}

template<typename T>
T foxtrot::devices::APT::request_response_struct(foxtrot::devices::bsc203_opcodes opcode_send,
						    foxtrot::devices::bsc203_opcodes opcode_recv,
						    destination dest, unsigned char p1, unsigned char p2,
						    std::optional<destination> expd_src
						)
{

    transmit_message(opcode_send, p1, p2, dest);
    return _response_struct_common<T>(opcode_recv, dest, expd_src);
}

template<typename T, typename arrtp>
T foxtrot::devices::APT::request_response_struct(bsc203_opcodes opcode_send, bsc203_opcodes opcode_recv, destination dest, arrtp& data, std::optional<destination> expd_src)
{
    transmit_message(opcode_send, data, dest);
    return _response_struct_common<T>(opcode_recv, dest, expd_src);
}

template<typename T>
std::array<unsigned char, 6> foxtrot::devices::get_move_request_header_data(T distance, foxtrot::devices::motor_channel_idents chan)
{
    unsigned char* distbytes = reinterpret_cast<unsigned char*>(&distance);
    std::array<unsigned char, 6> data {static_cast<unsigned char>(chan), 0, distbytes[0], distbytes[1], distbytes[2], distbytes[3]};

    return data;
}

using foxtrot::devices::apt_reply;
using foxtrot::devices::bsc203_opcodes;

template<typename It>
std::tuple<apt_reply, bsc203_opcodes> foxtrot::devices::APT::receive_message_sync_check(It&& expected_opcodes_begin, It&& expected_opcodes_end, destination expected_source, optional<milliseconds> timeout)
{
  auto [repl, recvd_opcode] = receive_sync_common(expected_source, timeout);

  auto pos = std::find_if(expected_opcodes_begin, expected_opcodes_end,
			  [recvd_opcode] (auto& c) { return static_cast<unsigned short>(c) == recvd_opcode;});

  if(pos == expected_opcodes_end)
    {
      _lg.strm(sl::error) << "received opcode: " << std::hex << recvd_opcode;



      std::ostringstream oss;
      for(auto it = expected_opcodes_begin; it !=expected_opcodes_end; it++)
	{
	  oss << "0x"<<std::hex << static_cast<unsigned short>(*it) << ",";
	}
      _lg.strm(sl::error) << "but expected one in: " << oss.str();

    }

  return {repl, static_cast<bsc203_opcodes>(recvd_opcode)};

}


template <typename T>
std::array<unsigned char, sizeof(T)> copy_struct_to_array(T &&strct)
{
  std::array<unsigned char, sizeof(T)> out;
  auto* ptr = reinterpret_cast<const unsigned char*>(&strct);
  std::copy(ptr, ptr+sizeof(T), out.begin());
  return out;

}
