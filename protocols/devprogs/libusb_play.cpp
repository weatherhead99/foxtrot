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

  //test to find single device
  //this is the VID, PID for an unloaded FX2 chip
  //  auto fx2_device =  find_single_device(VID, PID);
  
    
  // cout  << "n_devices found: " << devlist.n_devices() << endl;

  // cout << "------------------" << endl;
  // for(auto dev: devlist)
  //   {
  //     auto desc = dev.device_descriptor();
  //     cout << "vid: 0x" << std::hex << desc.idVendor << ", pid: 0x" << desc.idProduct << endl;
      
  //   }


  // const unsigned short VID = 0x047d;
  // const unsigned short PID = 0x1020;

  // LibUsbDeviceList devlist;
  // auto devit = devlist.cbegin();
  // int i= 0;
  // while(devit != devlist.cend())
  //   {
  //     cout << "i: " << i++ << endl;
  //     //find a kensington expert 
  //     devit = std::find_if(devit++, devlist.cend(),
  // 			   [VID, PID] (const auto& dev) {
  // 			     auto desc = dev.device_descriptor();
  // 			     return (desc.idVendor == VID) and (desc.idProduct == PID);
  // 			   });

  //     if(devit == devlist.cend())
  // 	cout << "no device found..." << endl;
  //     else
  // 	cout << "device found..." << endl;
  //   }
  
	     auto fx2_device =  find_single_device(VID, PID, true);
  if(fx2_device.has_value())
    cout << "found a device!" << endl;
  else
    cout << "didn't find device" << endl;
  
}
