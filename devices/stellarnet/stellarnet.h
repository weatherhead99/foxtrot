#pragma once
#include "Device.h"
#include "Logging.h"

#define FX2_VID 0x04B4
#define FX2_PID 0x8613

#define STELLARNET_VID 0x0BD7
#define STELLARNET_PID 0xA012

#define COEFF_DEVICE_ID_ADDR 0x20
#define COEFF_C1_ADDR 0x80
#define COEFF_C2_ADDR 0xA0
#define COEFF_C3_ADDR 0xC0
#define COEFF_C4_ADDR 0xE0

#include <rttr/registration>
#include "CommunicationProtocol.h"

class libusb_device;
class libusb_context;
class libusb_device_handle;
class libusb_device_descriptor;

namespace foxtrot
{
 namespace devices {
    
   class stellarnet : public Device
   {
     RTTR_ENABLE(Device)
   public:
     stellarnet(const std::string& firmware_file, int timeout_ms);
     stellarnet(const foxtrot::parameterset& params);
      virtual const std::string getDeviceTypeName() const;
      virtual ~stellarnet();
      
      std::vector<unsigned short> read_spectrum(int int_time_ms);
      //NOTE: wavelengths are calculated as p**3 * c3/8 + p**2 * c1/4 + p*c0/2 + c2
      std::vector<double> get_coeffs() const;
      
      
      
   private:
     
     void reenumerate_device(libusb_device_descriptor* desc, libusb_device* dev);
     void setup_reenumerated_device(libusb_device_descriptor* desc, libusb_device* dev);
     
     void set_device_timing(unsigned short tm, unsigned char x_timing);
     
     void set_device_config(short unsigned integration_time, unsigned char XTiming, short unsigned boxcar_smooth, int tempcomp);
     
     
     std::array<unsigned char,0x20> get_stored_bytes(unsigned int addr);
     
     libusb_context* _ctxt = nullptr;
     libusb_device_handle* _hdl = nullptr;
     
     Logging _lg;
     
     const std::string _firmware_file;
     
     int _timeout_ms;
     
     
     std::vector<double> _coeffs;
     
     std::string _dettype;
     std::string _devid;
     
     int _devidx;
     
     
   };
   
   
   
 }
  
  
  
}