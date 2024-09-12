#include <foxtrot/protocols/libUsbProtocol.hh>
#include <iostream>
#include <foxtrot/Logging.h>
#include <libusb.h>


using namespace foxtrot::protocols;
using std::cout;
using std::endl;

int main()
{

  foxtrot::setLogFilterLevel(sl::debug);
  
  LibUsbDeviceList devlist;
  cout  << "n_devices found: " << devlist.n_devices() << endl;
  
  for(int i=0; i < devlist.n_devices(); i++)
    {
      auto dev = devlist[i];
      auto vid = dev.device_descriptor().idVendor;
      auto pid = dev.device_descriptor().idProduct;
      cout << "vid: " << vid << ", pid: " << pid << endl;

      if(vid == 0x0BD7)
	{
	  cout << "stellarnet found!" << endl;
	  dev.open();
	}

    }

}
