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
  

  if(_dev == nullptr)
    throw foxtrot::ProtocolError("LibUsbProtocol internal device is nullptr for some reason, this shouoldn't happen!");

  std::vector<unsigned char> data;
  
  try{
    auto rt = std::chrono::milliseconds(_read_timeout);
    data = _dev->blocking_bulk_transfer_receive(_epin, len, rt);
  }
  catch(foxtrot::protocols::LibUsbError& err)
    {
      _lg.strm(sl::error) << "caught libusb error with code : " << err.code;
      throw foxtrot::ProtocolError(err.what());
    }

  if(actlen != nullptr)
    *actlen = data.size();

  std::string out(data.begin(), data.end());
  return out;
}


void foxtrot::protocols::BulkUSB::write(const std::string& data)
{


  if(_dev == nullptr)
    throw foxtrot::ProtocolError("LibUsbProtocol internal device is nullptr for some reason, this shouoldn't happen!");

  int actlen;
  
  try
    {
      auto wt = std::chrono::milliseconds(_write_timeout);
      unsigned char* udat = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data.c_str()));
      std::span<unsigned char> spdat(udat, data.size());
      
      _dev->blocking_bulk_transfer_send(_epout, spdat, wt, &actlen);
    }
  catch(foxtrot::protocols::LibUsbError& err)
    {
      _lg.strm(sl::error) << "caught libusb error with code: " << err.code;
      throw foxtrot::ProtocolError(err.what());
    }


  if(actlen != data.size())
  {
    throw ProtocolError("didn't write all of data!");  
  };
  
}

void foxtrot::protocols::BulkUSB::clear_halts()
{
    if(_dev == nullptr)
    throw foxtrot::ProtocolError("LibUsbProtocol internal device is nullptr for some reason, this shouoldn't happen!");

  
  _lg.Debug("attempting clearing halts on endpoints...");
  try
    {
      _dev->clear_halt(_epout);
      _dev->clear_halt(_epin);
    }
  catch(foxtrot::protocols::LibUsbError& err)
    {
      _lg.strm(sl::error) << "caught libusb error with code: " << err.code;
      throw foxtrot::ProtocolError(err.what());      
    }


}

