#include "M30X.hh"
#include "APT_defaultdest.hh"

using namespace foxtrot::protocols;
using namespace foxtrot::devices;

using foxtrot::devices::APTDeviceTraits;
using foxtrot::devices::M30X;
using foxtrot::devices::bsc203_opcodes;

M30X::M30X(shared_ptr<SerialPort> proto)
  : APT_DefaultDest(proto, destination::genericUSB), _lg("M30X")
{
  stop_update_messages(destination::genericUSB);
  _serport->flush();
  start_motor_messages(destination::genericUSB);
  //  start_update_messages(destination::genericUSB);

  APTDeviceTraits M30X_traits;
  M30X_traits.status_request_code = bsc203_opcodes::MGMSG_MOT_REQ_DCSTATUSUPDATE;
  M30X_traits.status_get_code = bsc203_opcodes::MGMSG_MOT_GET_DCSTATUSUPDATE;
  M30X_traits.motor_status_messages = {
      foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED,
      foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,
      foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_STOPPED,
      foxtrot::devices::bsc203_opcodes::MGMSG_MOT_GET_DCSTATUSUPDATE
  };

  M30X_traits.n_ignore_failed_motor_msgs = 10;

  _traits = M30X_traits;
}

void M30X::acknowledge_status_update()
{
  //NOTE: the manual says (somewhat ambiguously) that these acknowledgements might be needed, but
  // transmitting them seems to put the motor into an irrecoverable error condition. So we disable them for now
  //  std::array<int, 0> arr;
  //transmit_message(bsc203_opcodes::MGMSG_MOT_ACK_STATUSUPDATE, arr, destination::genericUSB, destination::genericUSB);

}

const std::string M30X::getDeviceTypeName() const
{
  return "M30X";
}


void M30X::absolute_move_blocking(motor_channel_idents channel, unsigned target)
{
  //NOTE this seems to need to go to bay 1, inexplicably
  //TODO: test if this is the case for both channels!
  APT::absolute_move_blocking(destination::bay1, channel, target);
}

void M30X::relative_move_blocking(motor_channel_idents channel, int target)
{
  APT::relative_move_blocking(destination::bay1, channel, target);
}

void M30X::home_move_blocking(motor_channel_idents channel)
{
  APT::home_move_blocking(destination::bay1, channel);
}


void M30X::safe_home_move(bool home_left, long velocity)
{
  // Check if the soft limits allow for a safe homing move
  if (!check_soft_limits(home_left))
  {
    _lg.strm(sl::error) << "Insufficient space for homing move. Operation aborted.";
    return;
  }


  // Retrieve current home parameters
  auto homeparms = get_homeparams(motor_channel_idents::channel_1);

  // Set the home direction and limit switch based on the input
  if (home_left) {
    homeparms.homeDir = 2;  // Set home direction to left (clockwise)
    homeparms.limitSwitch = 1; // Use left limit switch
  } else {
    homeparms.homeDir = 1;  // Set home direction to right (counter-clockwise)
    homeparms.limitSwitch = 4; // Use right limit switch
  }

  // Set the home velocity
  homeparms.homeVelocity = velocity;

  // Apply the updated home parameters
  set_homeparams(homeparms);

  _lg.strm(sl::debug) << "Homing with direction " << (home_left ? "left" : "right") << " and velocity " << velocity;

  // Execute the home move
  APT::home_move_blocking(destination::bay1, motor_channel_idents::channel_1);

  _lg.strm(sl::debug) << "Home move completed.";
}

void M30X::set_soft_limits(long CW_limit, long CCW_limit)
{
  auto limswitchparams = get_limitswitchparams(motor_channel_idents::channel_1);

  limswitchparams.CWsoft = CW_limit;
  limswitchparams.CCWsoft = CCW_limit;

  set_limitswitchparams(limswitchparams);

  _lg.strm(sl::debug) << "Soft limits set. CW: " << CW_limit << " CCW: " << CCW_limit;
}

bool M30X::check_soft_limits(bool home_left, long required_distance)
{
  auto limswitchparams = get_limitswitchparams(motor_channel_idents::channel_1);

  if (home_left) {
    // Check if there's enough space to the left (CW direction)
    return (limswitchparams.CWsoft >= required_distance);
  } else {
    // Check if there's enough space to the right (CCW direction)
    return (limswitchparams.CCWsoft <= -required_distance);
  }
}

RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::M30X;

  registration::class_<M30X>("foxtrot::devices::M30X");
}
