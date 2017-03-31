#include "n52.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <sstream>
#include <algorithm>
#include <array>

#define VENDOR_BELKIN 0x050d
#define DEVICE_N52 0x0815

foxtrot::n52::n52()
: _lg("n52")
{
  
  //find n52 device
  int fd;
  auto is_n52 = [this,&fd] (int i)
  {
    std::ostringstream oss;
    oss << "/dev/input/event" << i;
    
    struct input_id info;
    fd = open(oss.str().c_str(),O_RDWR);
    if(fd > 0)
    {
      //NOTE: failing to open RDWR isn't an error condition, necessarily
      if(ioctl(fd,EVIOCGID,&info) ==0)
      {
	if(info.vendor == VENDOR_BELKIN && info.product == DEVICE_N52)
	{
	  return true;
	}
	
      }
      close(fd);
    };
    
    return false;
    
  };
  
 
  bool found = false;
  for(int i= 0; i < 16; i++)
  {
    if(is_n52(i))
    {
      _lg.Debug("found n52");
      setup_n52_dev(fd);
      found=true;
      
    }
    
  };
  
  if(!found)
  {
  throw std::runtime_error("didn't find n52 keypad...");
  }
  
  
  
}



foxtrot::n52::~n52()
{
  close(_keypad_fd);
  close(_led_fd);

}

void foxtrot::n52::setup_n52_dev(int fd)
{
  if(auto ret = ioctl(fd, EVIOCGRAB,1) <0 )
  {
   throw std::runtime_error(std::string("error grabbing interface: ") + strerror( ret));  
  }
  
  //find LED control device
  unsigned char led_bitmask[LED_MAX/8 +1] = {0};
  if( auto ret = ioctl(fd,EVIOCGBIT(EV_LED,sizeof(led_bitmask)), led_bitmask) <0)
  {
    throw std::runtime_error("ioctl to find LED device failed");
  };
  
  if(!led_bitmask[0])
  {
    _lg.Debug("this is not the LED device");
    _keypad_fd = fd;
    return;
  }
  
  _lg.Debug("this is the LED device");
  _led_fd = fd;
    

}


void foxtrot::n52::set_led_state(bool blue, bool green, bool red)
{
  std::array<unsigned char,3> leds {blue,green,red};
  struct input_event ev;
  
  for(int i=0;i <3; i++)
  {
    ev.type = EV_LED;
    ev.code = i;
    ev.value=leds[i];
    
    if(auto ret = write(_led_fd,&ev,sizeof(ev))  < 0)
    {
      throw std::runtime_error(std::string("error writing LED state: ") + strerror(ret));
    }
    
  }
  

}

