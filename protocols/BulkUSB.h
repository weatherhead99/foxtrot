#pragma once
#include "SerialProtocol.h"
#include "Logging.h"

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
      
    virtual std::string read(unsigned int len, unsigned* actlen = nullptr) override;
    virtual void write(const std::string& data) override;
    
    
    private:
      libusb_context* _ctxt;
      libusb_device_handle* _hdl;
      int _vid;
      int _pid;
      int _epout;
      int _epin;
      int _write_timeout = 0;
      int _read_timeout = 0;
      foxtrot::Logging _lg;
    };
    
    
  }//namespace protocols
  
  
}//namespace foxtrot