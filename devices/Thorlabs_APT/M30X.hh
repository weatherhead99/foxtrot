#pragma once

#include "APT_defaultdest.hh"
#include <rttr/registration>
#include <memory>
#include <foxtrot/Logging.h>

using std::shared_ptr;

namespace foxtrot
{
  namespace protocols
  {
    class SerialPort;
  }

  namespace devices
  {
    using protocols::SerialPort;
    class M30X : public APT_DefaultDest
    {
      RTTR_ENABLE(APT_DefaultDest);

    public:
      explicit M30X(shared_ptr<SerialPort> proto);
      const std::string getDeviceTypeName() const override;

      void absolute_move_blocking(motor_channel_idents channel, unsigned target) override;
      void relative_move_blocking(motor_channel_idents channel, int target) override;
      void home_move_blocking(motor_channel_idents channel) override;

      
      void safe_home_move(bool home_left, long velocity=223749);
      void set_soft_limits(long CW_limit, long CCW_limit);

    protected:
      void acknowledge_status_update();

    private:
      foxtrot::Logging _lg;
      bool check_soft_limits(bool home_left, long required_distance = 170000); // Check soft limits before homing
    };
  }
}
