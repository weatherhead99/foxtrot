#pragma once
#include "SerialProtocol.h"

class libusb_context;
class libusb_device_handle;


#include <memory>

namespace foxtrot
{
  namespace protocols
  {
    class BulkUSB : public SerialProtocol
    {
    public:
    BulkUSB(const parameterset*const instance_parameters);
    virtual ~BulkUSB();
    virtual void Init(const parameterset*const class_parameters) override;
      
    virtual std::string read(unsigned int len) override;
    virtual void write(const std::string& data) override;
    
    private:
      libusb_context* _ctxt;
      libusb_device_handle* _hdl;
      unsigned _vid;
      unsigned _pid;
      unsigned _epout;
      unsigned _epin;
      unsigned _write_timeout;
      unsigned _read_timeout;
      
    };
    
    
  }//namespace protocols
  
  
}//namespace foxtrot