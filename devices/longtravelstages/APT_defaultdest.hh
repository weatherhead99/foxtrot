#pragma once

#include <foxtrot/devices/APT.h>
#include <foxtrot/protocols/SerialPort.h>
#include <memory>
#include <rttr/registration>

using std::shared_ptr;
using foxtrot::protocols::SerialPort;

namespace foxtrot
{
  namespace devices
  {
    class APT_DefaultDest : public APT
    {
      RTTR_ENABLE(APT);
      
    public:
      APT_DefaultDest(shared_ptr<SerialPort> proto,
		      destination dest=destination::genericUSB);

      hwinfo get_hwinfo();
      bool get_channelenable(motor_channel_idents chan);
      void set_channelenable(motor_channel_idents chan, bool onoff);

      allstatus get_status(motor_channel_idents channel);


      //NOTE: some stages have quirks where they need a different destination,
      //JUST for move commands. So make these virtual, but with default implementations
      virtual void absolute_move_blocking(motor_channel_idents channel, unsigned target);
      virtual void relative_move_blocking(motor_channel_idents channel, int target);
      virtual void home_move_blocking(motor_channel_idents channel);

      void set_velocity_params(const velocity_params& velpar);
      velocity_params get_velocity_params(motor_channel_idents channel);

      std::chrono::milliseconds estimate_abs_move_time(motor_channel_idents channel, unsigned int target, std::optional<unsigned int> start=std::nullopt);

      std::chrono::milliseconds estimate_rel_move_time(motor_channel_idents channel, int target);

      int get_position_counter(motor_channel_idents channel);
      void set_position_counter(motor_channel_idents channel, int val);

      homeparams get_homeparams(motor_channel_idents channel);
      void set_homeparams(const homeparams& params);

      limitswitchparams get_limitswitchparams(motor_channel_idents channel);
      void set_limitswitchparams(const limitswitchparams& params);
      
      
    private:
      destination _dest;
    };
  }
}
