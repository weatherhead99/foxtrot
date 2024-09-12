#pragma once
#include <foxtrot/Logging.h>
#include <foxtrot/protocols/SerialProtocol.h>
#include <foxtrot/ProtocolError.h>
#include <memory>
#include <optional>
#include <iterator>


class libusb_context;
class libusb_device_handle;
class libusb_device;
struct libusb_device_descriptor;

namespace foxtrot
{
  namespace protocols
  {

    class LibUsbError: public foxtrot::ProtocolError
    {
    public:
      LibUsbError(int errcode);
      LibUsbError(int errcode, foxtrot::Logging& lg);

      int code = 0;
      
    };

    class LibUsbDevice
    {
      friend class LibUsbDeviceList;
    public:
      ~LibUsbDevice();

      const libusb_device_descriptor& device_descriptor() const;
    private:
      LibUsbDevice(libusb_device* pdev);
      libusb_device* _devptr;
      libusb_device_descriptor* _devdesc;
    };

    class LibUsbDeviceList
    { 
    public:

      class const_iterator
      {
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;

      };
      
      LibUsbDeviceList();
      ~LibUsbDeviceList();

      
      LibUsbDevice operator[](std::size_t pos);
      int n_devices() const;
      std::optional<std::size_t> find_one_device(unsigned vid, unsigned pid);
      
    private:
      struct _LibUsbDeviceListDeleter
      {
	void operator()(libusb_device** list);
      };
      libusb_context* _ctxt = nullptr;
      std::size_t _n_devices;
      std::unique_ptr<libusb_device*, _LibUsbDeviceListDeleter> devlist;
    };
    
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
