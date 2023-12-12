#include <foxtrot/protocols/libUsbProtocol.hh>
#include <libusb.h>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>
#include <foxtrot/protocols/ProtocolUtilities.h>

using namespace foxtrot::protocols;

libUsbProtocol::libUsbProtocol(const parameterset *const instance_parameters)
    : SerialProtocol(instance_parameters), _lg("libUsbProtocol"){

}

libUsbProtocol::~libUsbProtocol()
{
  if(_hdl != nullptr)
    libusb_close(_hdl);
  
  libusb_exit(_ctxt);
}


void libUsbProtocol::Init(const foxtrot::parameterset*const class_parameters)
{
  //call base class to merge parameters
    foxtrot::CommunicationProtocol::Init(class_parameters);
  
    extract_parameter_value(_vid,_params,"vid");
    extract_parameter_value(_pid,_params,"pid");
    extract_parameter_value(_epin,_params,"epin");
    extract_parameter_value(_epout,_params,"epout");
    extract_parameter_value(_write_timeout,_params,"write_timeout", false);
    extract_parameter_value(_read_timeout,_params,"read_timeout",false);
    
    
    //get usb device
    auto free_devlist = [] (libusb_device** list) { libusb_free_device_list(list, true);};
    
    _lg.Info("reading device list...");
    
    libusb_device** listptr;
    
//     std::cout << "getting device list" << std::endl;
    auto num_devs = libusb_get_device_list(_ctxt, &listptr ); 
    
//     std::cout << "matching devices: " << num_devs << std::endl;
    
    std::vector<libusb_device*> matching_devices;
    matching_devices.reserve(num_devs);
    
    for(auto i=0 ; i < num_devs; i++)
    {
//       std::cout << "i: " << i << std::endl;
      libusb_device_descriptor desc;
      int ret;
      if( (ret = libusb_get_device_descriptor(*(listptr +i), &desc)) < 0)
      {
	_lg.Error("error getting device descriptor");
	throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
      };
      
      if(desc.idVendor == _vid && desc.idProduct == _pid)
      {
	matching_devices.push_back( *(listptr +i));
      };
        
    };
    
    if(matching_devices.size() == 0)
    {
      throw ProtocolError("no matching device connected");
    };
    
    
    if(matching_devices.size() > 1)
    {
      throw ProtocolError("multiple device support not implemented yet!"); 
    };
    
    
    
    int ret;
    if(  (ret = libusb_open(matching_devices[0], &_hdl) ) < 0)
    {
      _lg.Error("error opening device...");
      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    };
    
    if( (ret = libusb_reset_device(_hdl)) < 0)
    {
      _lg.Error("error resetting device...");
      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    }
    
    
    if( (ret = libusb_set_configuration(_hdl,1)) <0)
    {
      _lg.Error("error setting configuration");
      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    };
    
    if( (ret = libusb_claim_interface(_hdl,0)) < 0)
    {
      _lg.Error("error claiming interface");
      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    }
}

void libUsbProtocol::set_read_timeout(int timeout_ms)
{
  _read_timeout = timeout_ms;
}

int libUsbProtocol::get_read_timeout() const
{
  return _read_timeout;
}
  
void libUsbProtocol::set_write_timeout(int timeout_ms)
{
  _write_timeout = timeout_ms;
}

int libUsbProtocol::get_write_timeout() const
{
  return _write_timeout;
}



