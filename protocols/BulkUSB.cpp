#include "BulkUSB.h"

#include "ProtocolUtilities.h"

#include <libusb.h>
#include "ProtocolError.h"
#include <string>
#include <memory>

#include <vector>

foxtrot::protocols::BulkUSB::BulkUSB(const foxtrot::parameterset*const instance_parameters): SerialProtocol(instance_parameters)
{
  
  int ret;
  if( ( ret = libusb_init(&_ctxt)) < 0)
  {
    throw  ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  };
  

}

foxtrot::protocols::BulkUSB::~BulkUSB()
{
  
  if(_hdl != nullptr)
  {
    libusb_close(_hdl);
  }
  
  libusb_exit(_ctxt);
}


void foxtrot::protocols::BulkUSB::Init(const foxtrot::parameterset*const class_parameters)
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
    
    std::cout << "device list" << std::endl;
    
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
      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    };
    
    if( (ret = libusb_set_configuration(_hdl,1)) <0)
    {
      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    };
    
    
   
}


std::string foxtrot::protocols::BulkUSB::read(unsigned int len, unsigned* actlen)
{
  
  unsigned char* data = new unsigned char[len];
  
  
  int alen_loc;
  
  auto err = libusb_bulk_transfer(_hdl,_epin, data,len,&alen_loc,_read_timeout);
  if(err < 0)
  {
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(err)));
  };
  
  if(actlen != nullptr)
  {
    *actlen = alen_loc; 
  }
  
  return std::string(data, (data + alen_loc));

}


void foxtrot::protocols::BulkUSB::write(const std::string& data)
{
  int act_len;
  auto err = libusb_bulk_transfer(_hdl,_epout,reinterpret_cast<unsigned char*>(const_cast<char*>(data.data())),data.size(),&act_len,_write_timeout);
  if(err < 0)
  {
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(err)));
  };
  if(act_len != data.size())
  {
    throw ProtocolError("didn't write all of data!");  
  };
  
}
