#pragma once

namespace foxtrot
{
  template <typename DeviceType>
  class Repairable
  {
  public:
    template<typename... Ts>
    Repairable(Ts...) : DeviceType(std::forward<Ts>(args)...)
    {};

    virtual void reset() = 0;
    virtual void reset_comms() = 0;

  };
  
}
