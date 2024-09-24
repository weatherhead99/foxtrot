#include <foxtrot/protocols/libUsbProtocol.hh>
#include <iostream>
#include <foxtrot/Logging.h>
#include <libusb.h>
#include <algorithm>
#include <optional>

using namespace foxtrot::protocols;
using std::cout;
using std::endl;



int main()
{
  unsigned short VID = 0x04B4;
  unsigned short PID = 0x8613;

  foxtrot::setLogFilterLevel(sl::trace);


  LibUsbContext ctxt;
  
  auto fx2_device =  find_single_device(ctxt, VID, PID, true);
  if(fx2_device.has_value())
    cout << "found a device!" << endl;
  else
    {
      cout << "didn't find device" << endl;
      return 0;
    }

  auto dev = *fx2_device;

  cout << "is open: " << dev.is_open() << endl;
  dev.open();

  cout << "is open: " << dev.is_open() << endl;

  cout << "configuration: " << dev.configuration() << endl;

  
  
  
}
