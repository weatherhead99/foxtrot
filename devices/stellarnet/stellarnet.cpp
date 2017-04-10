#include "stellarnet.h"
#include "ProtocolError.h"
#include "DeviceError.h"
#include <libusb.h>

#define DEVICE_ID_ADDR 0x20
#define COEFF_C1_ADDR 0x80
#define COEFF_C2_ADDR 0xA0
#define COEFF_C3_ADDR 0xC0
#define COEFF_C4_ADDR 0xE0




foxtrot::devices::stellarnet::stellarnet(const std::string& firmware_file): CmdDevice(nullptr),
_lg("stellarnet"), _firmware_file(firmmware_file)
{
  if( int ret = libusb_init(&_ctxt) < 0)
  {
   _lg.Error("error initializing libusb");
  throw  ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret))); 
  }
  
  //device list
  auto free_devlist = [] (libusb_device** list) { libusb_free_device_list(list, true);};
  _lg.Info("reading device list...");
  
  libusb_device** listptr;
  auto num_devs = libusb_get_device_list(_ctxt,&listptr);
  
  std::vector<libusb_device*> matching_devices;
  matching_devices.reserve(num_devs);
  
  
  bool second_run = true;
  
  while(second_run)
  {
    for(auto i =0; i< num_devs; i++)
    {
      libusb_device_descriptor desc;
      if( int ret = libusb_get_device_descriptor(*(listptr+i), &desc) < 0)
      {
	_lg.Error("error getting device descriptor"); 
	throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
      };
      
      if(desc.idVendor == FX2_VID && desc.idProduct == FX2_PID)
      {
	_lg.Info("found empty FX2 chip, uploading firmware...");
	reenumerate_device(desc,*(listptr+i));
	//NOTE: loop will run again and init real device
	
      }
      else if(desc.idVendor == STELLARNET_VID && desc.idProduct == STELLARNET_PID)
      {
	_lg.Info("found enumerated stellarnet device!");
	setup_reenumerated_device(desc, *(listptr+i));
	
	break;
      }
      
    }
    
    _lg.Error("no stellarnet device found...");
    throw DeviceError("no stellarnet device found!");
  }
    
    libusb_free_device_list(listptr,true);

}


foxtrot::devices::stellarnet::~stellarnet()
{
  if(_hdl != nullptr)
  {
    libusb_close(_hdl);
  }
  
  libusb_exit(_ctxt);

}


const std::string foxtrot::devices::stellarnet::getDeviceTypeName() const
{
  return "stellarnet";
}


foxtrot::devices::stellarnet::reenumerate_device(libusb_device_descriptor* desc, libusb_device* dev)
{
  
  //set configuration
  if(libusb_open(dev,_hdl) < 0)
  {
    _lg.Error("error opening device...");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }

  int cfg;
  if( libusb_get_configuration(_hdl,&cfg) < 0)
  {
    _lg.Error("error checking active config..>");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  if(cfg != 1)
  {
   libusb_set_configuration(_hdl,1); 
  }
  
  //load firmware
  
  
  
  
  
  

}

foxtrot::devices::stellarnet::setup_reenumerated_device(libusb_device_descriptor* desc, libusb_device* dev)
{

}

