#include <foxtrot/protocols/libUsbProtocol.hh>
#include <iostream>
#include <foxtrot/Logging.h>
#include <libusb.h>
#include <algorithm>


using namespace foxtrot::protocols;
using std::cout;
using std::endl;

int main()
{

  foxtrot::setLogFilterLevel(sl::debug);
  
  LibUsbDeviceList devlist;
  cout  << "n_devices found: " << devlist.n_devices() << endl;

  cout << "------------------" << endl;
  for(auto dev: devlist)
    {
      auto desc = dev.device_descriptor();
      cout << "vid: 0x" << std::hex << desc.idVendor << ", pid: 0x" << desc.idProduct << endl;
      
    }


  const unsigned short VID = 0x047d;
  const unsigned short PID = 0x1020;

  auto devit = devlist.cbegin();
  int i= 0;
  while(devit != devlist.cend())
    {
      cout << "i: " << i++ << endl;
      //find a kensington expert 
      devit = std::find_if(devit++, devlist.cend(),
			   [VID, PID] (const auto& dev) {
			     auto desc = dev.device_descriptor();
			     return (desc.idVendor == VID) and (desc.idProduct == PID);
			   });

      if(devit == devlist.cend())
	cout << "no device found..." << endl;
      else
	cout << "device found..." << endl;
    }
  
  
}
