#include "stellarnet.h"
#include "ProtocolError.h"
#include "DeviceError.h"
#include <libusb.h>
#include <fstream>


#define DEVICE_ID_ADDR 0x20
#define COEFF_C1_ADDR 0x80
#define COEFF_C2_ADDR 0xA0
#define COEFF_C3_ADDR 0xC0
#define COEFF_C4_ADDR 0xE0




foxtrot::devices::stellarnet::stellarnet(const std::string& firmware_file, int timeout_ms): CmdDevice(nullptr),
_lg("stellarnet"), _firmware_file(firmware_file), _timeout_ms(timeout_ms)
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
	reenumerate_device(&desc,*(listptr+i));
	//NOTE: loop will run again and init real device
	
      }
      else if(desc.idVendor == STELLARNET_VID && desc.idProduct == STELLARNET_PID)
      {
	_lg.Info("found enumerated stellarnet device!");
	setup_reenumerated_device(&desc, *(listptr+i));
	
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


void foxtrot::devices::stellarnet::reenumerate_device(libusb_device_descriptor* desc, libusb_device* dev)
{
  
  //set configuration
  if(int ret = libusb_open(dev,&_hdl) < 0)
  {
    _lg.Error("error opening device...");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }

  int cfg;
  if( int ret = libusb_get_configuration(_hdl,&cfg) < 0)
  {
    _lg.Error("error checking active config..>");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  if(cfg != 1)
  {
   libusb_set_configuration(_hdl,1); 
  }
  
  auto reqtp_out = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
  auto reqtp_in = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
  //load firmware
  
  //CPU into reset
  unsigned char reset_state[1] = {1};
  if(int ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,0xE600,0,reset_state,1,_timeout_ms) < 0)
  {
    _lg.Error("error putting CPU into reset");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  //load firmware
  std::ifstream ifs(_firmware_file);
  std::string line;
  while(std::getline(ifs,line))
  {
    int count = std::stoi(line.substr(0,2),0,16);
    int addr = std::stoi(line.substr(3,4),0,16);
    
    auto payloadstr = line.substr(9,count*2);
    
    std::vector<unsigned char> payload;
    payload.resize(count);
     
    auto striter = payloadstr.begin();
    for(auto& c_out : payload)
    {
      c_out = std::stoi(std::string(*striter, *striter+2),0,16);
      striter+=2;
    };
    
    if(auto ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,addr,0,payload.data(),payload.size(),_timeout_ms) < 0)
    {
     _lg.Error("error loading firmware at address: " + std::to_string(addr));
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    }
    
  };
  
  
  
  //CPU out of reset
  reset_state[0] = 0;
  if(auto ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,0xE600,0,reset_state,1,_timeout_ms) < 0)
  {
    _lg.Error("error taking CPU out of reset");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  
  
  

}

void foxtrot::devices::stellarnet::setup_reenumerated_device(libusb_device_descriptor* desc, libusb_device* dev)
{

}

