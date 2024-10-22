#include "APT_defaultdest.hh"

using foxtrot::devices::APT_DefaultDest;
using namespace foxtrot::devices;

APT_DefaultDest::APT_DefaultDest(shared_ptr<SerialPort> proto, destination dest) : _dest(dest), APT(proto)
{

}


hwinfo APT_DefaultDest::get_hwinfo() { return APT::get_hwinfo(_dest); }

bool APT_DefaultDest::get_channelenable(motor_channel_idents chan)
{
  return APT::get_channelenable(_dest, chan);
}

void APT_DefaultDest::set_channelenable(motor_channel_idents chan, bool onoff)
{
  APT::set_channelenable(_dest, chan, onoff);
}

allstatus APT_DefaultDest::get_status(motor_channel_idents chan)
{
  //make sure to call override version
  return static_cast<APT*>(this)->get_status(_dest, chan);
}

void APT_DefaultDest::absolute_move_blocking(motor_channel_idents channel, unsigned target)
{
  APT::absolute_move_blocking(_dest, channel, target);
}

void APT_DefaultDest::relative_move_blocking(motor_channel_idents channel, int target)
{
  APT::relative_move_blocking(_dest, channel, target);
}

void APT_DefaultDest::home_move_blocking(motor_channel_idents channel)
{
  APT::home_move_blocking(_dest, channel);
}

void APT_DefaultDest::set_velocity_params(const velocity_params& velpar)
{
  APT::set_velocity_params(_dest, velpar);
}

velocity_params APT_DefaultDest::get_velocity_params(motor_channel_idents channel)
{
  return APT::get_velocity_params(_dest, channel);
}

std::chrono::milliseconds APT_DefaultDest::estimate_abs_move_time(motor_channel_idents channel, unsigned int target, std::optional<unsigned int> start)
{
  return APT::estimate_abs_move_time(_dest, channel, target,start);
}

std::chrono::milliseconds APT_DefaultDest::estimate_rel_move_time(motor_channel_idents channel,  int target)
{
  return APT::estimate_rel_move_time(_dest, channel, target);
}

int APT_DefaultDest::get_position_counter(motor_channel_idents channel)
{
  return APT::get_position_counter(_dest, channel);
}

void APT_DefaultDest::set_position_counter(motor_channel_idents channel, int val)
{
  APT::set_position_counter(_dest, channel, val);
}

homeparams APT_DefaultDest::get_homeparams(motor_channel_idents channel)
{
  return APT::get_homeparams(_dest, channel);
}

void APT_DefaultDest::set_homeparams(const homeparams& params)
{
  APT::set_homeparams(_dest, params);
}

limitswitchparams APT_DefaultDest::get_limitswitchparams(motor_channel_idents channel)
{
  return APT::get_limitswitchparams(_dest, channel);
}

void APT_DefaultDest::set_limitswitchparams(const limitswitchparams& params)
{
  APT::set_limitswitchparams(_dest, params);
}



RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::APT_DefaultDest;

  registration::class_<APT_DefaultDest>("foxtrot::devices::APT_DefaultDest")
    .property_readonly("hwinfo", &APT_DefaultDest::get_hwinfo)
    .method("get_channelenable", &APT_DefaultDest::get_channelenable)
    (parameter_names("chan"))
    .method("set_channelenable", &APT_DefaultDest::set_channelenable)
    (parameter_names("chan", "onoff"))
    .method("get_status", &APT_DefaultDest::get_status)
    (parameter_names("channel"))
    .method("absolute_move_blocking", &APT_DefaultDest::absolute_move_blocking)
    (parameter_names("channel", "target"))
    .method("relative_move_blocking", &APT_DefaultDest::relative_move_blocking)
    (parameter_names("channel", "target"))
    .method("home_move_blocking", &APT_DefaultDest::home_move_blocking)
    (parameter_names("channel"))
    .method("set_velocity_params", &APT_DefaultDest::set_velocity_params)
    (parameter_names("velpar"))
    .method("get_velocity_params", &APT_DefaultDest::get_velocity_params)
    (parameter_names("channel"))
    .method("get_position_counter", &APT_DefaultDest::get_position_counter)
    (parameter_names("channel"))
    .method("set_position_counter", &APT_DefaultDest::set_position_counter)
    (parameter_names("channel", "val"))
    .method("get_homeparams", &APT_DefaultDest::get_homeparams)
    (parameter_names("channel"))
    .method("set_homeparams", &APT_DefaultDest::set_homeparams)
    (parameter_names("params"))
    .method("get_limitswitchparams", &APT_DefaultDest::get_limitswitchparams)
    (parameter_names("channel"))
    .method("set_limitswitchparams", &APT_DefaultDest::set_limitswitchparams)
    (parameter_names("params"))
     
    ;

}

