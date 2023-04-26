#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include <libusb.h>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>

#include <foxtrot/protocols/BulkUSB.h>
#include <foxtrot/protocols/ProtocolUtilities.h>


foxtrot::protocols::BulkUSB::BulkUSB(const foxtrot::parameterset* const instance_parameters)
  : libUsbProtocol(instance_parameters), _lg("BulkUSB")
{
 
}

foxtrot::protocols::BulkUSB::~BulkUSB()
{
  
}




std::string foxtrot::protocols::BulkUSB::read(unsigned int len, unsigned* actlen)
{
  
  unsigned char* data = new unsigned char[len];
  
  
  int alen_loc;
  
  auto err = libusb_bulk_transfer(_hdl,_epin, data,len,&alen_loc,_read_timeout);
  
  if(actlen != nullptr)
  {
    *actlen = alen_loc; 
  }
  
  if(err < 0)
  {
    _lg.Error("error doing bulk transfer for read");
    _lg.Debug("error code: " + std::to_string(err));
    if(err == -7)
    {
      throw ProtocolTimeoutError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(err)));
    }
    
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(err)));
  };
  
  std::string out(data, (data + alen_loc));
  
  delete[] data;
  return std::move(out);

}


void foxtrot::protocols::BulkUSB::write(const std::string& data)
{
  int act_len;
  auto err = libusb_bulk_transfer(_hdl,_epout,reinterpret_cast<unsigned char*>(const_cast<char*>(data.data())),data.size(),&act_len,_write_timeout);
  if(err < 0)
  {
    _lg.Error("error doing bulk transfer for write");
    _lg.Debug("error code:  " + std::to_string(err));
    if(err == -7)
    {
      throw ProtocolTimeoutError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(err)));
    }
    
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(err)));
  };
  if(act_len != data.size())
  {
    throw ProtocolError("didn't write all of data!");  
  };
  
}

void foxtrot::protocols::BulkUSB::clear_halts()
{
  _lg.Debug("attempting clearing halts on endpoints...");
  int ret;
  if( (ret = libusb_clear_halt(_hdl,_epout)) < 0)
  {
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  if( ( ret = libusb_clear_halt(_hdl,_epin)) < 0)
  {
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }

}

