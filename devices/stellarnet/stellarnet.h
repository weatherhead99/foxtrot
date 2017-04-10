#pragma once
#include <CmdDevice.h>
#include "Logging.h"

#define FX2_VID 0x04B4
#define FX2_PID 0x8613

#define STELLARNET_VID 0x0BD7
#define STELLARNET_PID 0xA012


class libusb_device;
class libusb_context;
class libusb_device_handle;
class libusb_device_descriptor;

namespace foxtrot
{
 namespace devices {
    
   class stellarnet : public CmdDevice
   {
   public:
     stellarnet(const std::string& firmware_file);
      virtual const std::string getDeviceTypeName() const;
      virtual ~stellarnet();
      
   private:
     
     reenumerate_device(libusb_device_descriptor* desc, libusb_device* dev);
     setup_reenumerated_device(libusb_device_descriptor* desc, libusb_device* dev);
     
     libusb_context* _ctxt;
     libusb_device_handle* _hdl;
     
     Logging _lg;
     
     const std::string _firmware_file;
     
   };
   
   
   
 }
  
  
  
}