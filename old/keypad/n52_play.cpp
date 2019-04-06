#include "n52.h"
#include <iostream>
#include "Logging.h"
#include <thread>
#include <chrono>


int main(int argc, char** argv)
{
  foxtrot::setDefaultSink();
  foxtrot::n52 keypad;
  
  keypad.set_led_state(0,0,1);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  keypad.set_led_state(0,1,0);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  keypad.set_led_state(1,0,0);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  keypad.set_led_state(1,1,0);
  

}