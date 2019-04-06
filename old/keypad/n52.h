#pragma once
#include "Logging.h"

namespace foxtrot
{

  class n52
  {
  public:
    n52();
    ~n52();
    
    void set_led_state(bool blue, bool green, bool red);
    
  private:
    
    void setup_n52_dev(int fd);
    
    foxtrot::Logging _lg;
    int _keypad_fd;
    int _led_fd;
  };




}