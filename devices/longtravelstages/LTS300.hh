#pragma once

#include "APT_defaultdest.hh"
#include <foxtrot/protocols/SerialPort.h>
#include <rttr/registration>
#include <memory>


using foxtrot::devices::dcstatus;
using foxtrot::devices::homeparams;
using foxtrot::devices::velocity_params;
    


namespace foxtrot
{
  namespace devices
  {
    using std::shared_ptr;

    
    class LTS300 : public APT_DefaultDest
    {

      RTTR_ENABLE(APT);
    public:


      const std::string getDeviceTypeName() const override;
      
      LTS300(shared_ptr<protocols::SerialPort> proto);

      channel_status get_status();

      bool get_channelenable();
      void set_channelenable(bool onoff);

      void absolute_move_blocking(unsigned int target);
      void relative_move_blocking(int target);
      void home_move_blocking();

      void set_velocity_params(velocity_params& velpar);
      velocity_params get_velocity_params();

      int get_position_counter();
      void set_position_counter(int val);

      homeparams get_homeparams();
      void set_homeparams(homeparams& params);

      limitswitchparams get_limitswitchparams();
      void set_limitswitchparams(limitswitchparams& params);


      void attempt_recover_serial_comms();
      
      
    };
  }
}
