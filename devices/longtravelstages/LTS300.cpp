#include "LTS300.hh"

#include <foxtrot/Logging.h>

using namespace foxtrot::devices;
using namespace foxtrot::protocols;

using foxtrot::devices::APTDeviceTraits;

LTS300::LTS300(shared_ptr<SerialPort> proto) : APT_DefaultDest(proto)
{
  stop_update_messages(destination::genericUSB);
  _serport->flush();
  
  _traits.require_enable_updates_for_status = true;
  
};


const std::string LTS300::getDeviceTypeName() const
{
  return "Thorlabs:LTS300";
}


channel_status LTS300::get_status()
{
  
  _lg.strm(sl::debug) << "in LTS300::get_status";
  
  return std::get<channel_status>(APT_DefaultDest::get_status(motor_channel_idents::channel_1));
}

bool LTS300::get_channelenable()
{
  return APT_DefaultDest::get_channelenable(motor_channel_idents::channel_1);
}

void LTS300::set_channelenable(bool onoff)
{
  APT_DefaultDest::set_channelenable(motor_channel_idents::channel_1, onoff);
}

void LTS300::absolute_move_blocking(unsigned int target)
{
  APT_DefaultDest::absolute_move_blocking(motor_channel_idents::channel_1, target);
}


void LTS300::relative_move_blocking(int target)
{
  APT_DefaultDest::relative_move_blocking(motor_channel_idents::channel_1, target);

};

void LTS300::home_move_blocking()
{
  APT_DefaultDest::home_move_blocking(motor_channel_idents::channel_1);

}

homeparams LTS300::get_homeparams()
{
  return APT_DefaultDest::get_homeparams(motor_channel_idents::channel_1);

}

void LTS300::set_homeparams(homeparams& params)
{
  params.chan_ident = static_cast<unsigned short>(motor_channel_idents::channel_1);
  APT_DefaultDest::set_homeparams(params);

}

limitswitchparams LTS300::get_limitswitchparams()
{
  return APT_DefaultDest::get_limitswitchparams(motor_channel_idents::channel_1);
  
}

void LTS300::set_limitswitchparams(limitswitchparams& params)
{
  params.chan_ident = static_cast<unsigned short>(motor_channel_idents::channel_1);
  APT_DefaultDest::set_limitswitchparams(params);

}

velocity_params LTS300::get_velocity_params()
{
  return APT_DefaultDest::get_velocity_params(motor_channel_idents::channel_1);

}

void LTS300::set_velocity_params(velocity_params& params)
{
  params.chan_ident = static_cast<unsigned short>(motor_channel_idents::channel_1);
  APT_DefaultDest::set_velocity_params(params);
}


void LTS300::set_position_counter(int val)
{
  APT_DefaultDest::set_position_counter(motor_channel_idents::channel_1, val);
  
}

int LTS300::get_position_counter()
{
  return APT_DefaultDest::get_position_counter(motor_channel_idents::channel_1);

}

void LTS300::attempt_recover_serial_comms()
{
  foxtrot::Logging lg("LTS300_serial_port_recovery");
  lg.strm(sl::info) << "attempting to recover serial comms by flushing...";

  auto bavailable = _serport->bytes_available();
  lg.strm(sl::info) << "there are: " << std::dec << bavailable << " bytes available";

  _serport->flush();
  lg.strm(sl::info) << "serial port flush completed";
}
  



RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::LTS300;

  registration::class_<LTS300>("foxtrot::devices::LTS300")
    .property("channelenable", &LTS300::get_channelenable,
	      &LTS300::set_channelenable)
    .property_readonly("hwinfo", &LTS300::get_hwinfo)
    .property_readonly("status", &LTS300::get_status)

    //NOTE: these are methods until we sort out properties with refs    //NOTE: these are methods until we sort out properties with refs
    .method("absolute_move_blocking_onechan", &LTS300::absolute_move_blocking)
    .method("relative_move_blocking_onechan", &LTS300::relative_move_blocking)
    .method("home_move_blocking_onechan", &LTS300::home_move_blocking)


    //NOTE: 2 rename single channel overrides
    .method("get_velocity_params_onechan", &LTS300::get_velocity_params)
    .method("set_velocity_params_onechan", &LTS300::set_velocity_params)
    (parameter_names("params"))

    .method("get_homeparams_onechan", &LTS300::get_homeparams)
    .method("set_homeparams_onechan", &LTS300::set_homeparams)
    (parameter_names("params"))

    //doesn't work due to reference vs value type I think
    //    .property("homeparams", &LTS300::get_homeparams, &LTS300::set_homeparams)
    
    .method("get_limitswitchparams_onechan", &LTS300::get_limitswitchparams)
    .method("set_limitswitchparams_onechan", &LTS300::set_limitswitchparams)
    (parameter_names("params"))

    //doesn't work due to reference vs value type I think
    //    .property("limitswitchparams", &LTS300::get_limitswitchparams, &LTS300::set_limitswitchparams)
    
    .property("position_counter", &LTS300::get_position_counter,
	      &LTS300::set_position_counter)

    .method("attempt_recover_serial_comms", &LTS300::attempt_recover_serial_comms);
    
    ;


}
