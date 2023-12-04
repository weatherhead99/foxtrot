#pragma once
#include <foxtrot/Logging.h>
#include <foxtrot/protocols/SerialProtocol.h>

class libusb_context;
class libusb_device_handle;

namespace foxtrot
{
  namespace protocols
  {
    class libUsbProtocol : public SerialProtocol
    {
    public:
      libUsbProtocol(const parameterset* const instance_parameters);
      virtual void Init(const parameterset*const class_parameters) override;

      void set_read_timeout(int timeout_ms);
      int get_read_timeout() const;
      void set_write_timeout(int timeout_ms);
      int get_write_timeout() const;
      
      
      virtual ~libUsbProtocol();

    protected:
      libusb_context* _ctxt = nullptr;
      libusb_device_handle* _hdl = nullptr;
      int _vid;
      int _pid;
      int _epout;
      int _epin;
      int _write_timeout = 0;
      int _read_timeout = 0;


    private:
      foxtrot::Logging _lg;
    };
  }
}
