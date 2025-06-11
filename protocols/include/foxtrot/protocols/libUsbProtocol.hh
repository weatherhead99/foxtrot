#pragma once
#include <foxtrot/Logging.h>
#include <foxtrot/protocols/SerialProtocol.h>
#include <foxtrot/ProtocolError.h>
#include <memory>
#include <optional>
#include <iterator>
#include <cstdint>
#include <span>
#include <chrono>
#include <vector>

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

    class LibUsbContext
    {

      friend class LibUsbDeviceList;
      
    public:
      LibUsbContext();
      ~LibUsbContext();

      LibUsbContext(LibUsbContext&& other);
      LibUsbContext(const LibUsbContext& other) = delete;
    private:
      libusb_context* ptr();
      libusb_context* _ptr;

    };
    

    class LibUsbDevice
    {
      friend class LibUsbDeviceList;
    public:
      ~LibUsbDevice();
      LibUsbDevice(const LibUsbDevice& other);
      LibUsbDevice(LibUsbDevice&& other);

      libusb_device_descriptor device_descriptor() const;

      bool is_open() const;
      void open();
      void set_configuration(int configuration);
      int configuration();

      void clear_halt(unsigned short endpoint);

      void close() noexcept;

      void reset();

      void claim_interface(std::uint8_t iface);
      void release_interface();


      std::vector<unsigned char> blocking_control_transfer_receive(std::uint8_t bmRequestType,
								 std::uint8_t bRequest,
								 std::uint16_t wValue,
								 std::uint16_t wIndex,
								 std::uint16_t wLength,
								 std::chrono::milliseconds timeout);

      void blocking_control_transfer_send(std::uint8_t bmRequestType,
					  std::uint8_t bRequest,
					  std::uint16_t wValue,
					  std::uint16_t wIndex,
					  const std::span<unsigned char> data,
					  std::chrono::milliseconds timeout);

      std::vector<unsigned char> blocking_bulk_transfer_receive(unsigned char endpoint,
								int maxlen,
								std::chrono::milliseconds timeout);

      void blocking_bulk_transfer_send(unsigned char endpoint,
				       const std::span<unsigned char> data,
				       std::chrono::milliseconds timeout,
				       int* transferred = nullptr);

      //TODO: fixed size receives!

    private:
      LibUsbDevice(libusb_device* pdev);
      libusb_device* _devptr;
      libusb_device_handle* _hdl = nullptr;
      std::optional<std::uint8_t> claimed_interface = std::nullopt;
    };

    class LibUsbDeviceList
    {
    public:

      class const_iterator
      {
	friend class LibUsbDeviceList;
      public:
	using iterator_category = std::input_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = LibUsbDevice;
	using pointer = const LibUsbDevice* const;
	using reference = const LibUsbDevice&;

	value_type operator*();

	const_iterator& operator++();
	const_iterator operator++(int);

	friend bool operator==(const const_iterator& a, const const_iterator& b);
	friend bool operator!=(const const_iterator& a, const const_iterator& b);

      private:
	explicit const_iterator(const LibUsbDeviceList* devlist, int pos);
	int _pos;
	const LibUsbDeviceList* _devlist;

      };

      LibUsbDeviceList(LibUsbContext& ctxt);
      ~LibUsbDeviceList();

      const_iterator cbegin() const;
      const_iterator cend() const;

      const_iterator begin() const;
      const_iterator end() const;

      LibUsbDevice operator[](std::size_t pos) const;
      int n_devices() const;

    private:
      struct _LibUsbDeviceListDeleter
      {
	void operator()(libusb_device** list);
      };
      
      std::size_t _n_devices;
      std::unique_ptr<libusb_device*, _LibUsbDeviceListDeleter> devlist;
    };

    bool operator==(const LibUsbDeviceList::const_iterator& a, const LibUsbDeviceList::const_iterator& b);
    bool operator!=(const LibUsbDeviceList::const_iterator& a, const LibUsbDeviceList::const_iterator& b);


    std::optional<LibUsbDevice> find_single_device(LibUsbContext& ctxt, unsigned short vid, unsigned short pid,
						   bool throw_on_multi=true);


    class libUsbProtocol : public SerialProtocol
    {
    public:
      libUsbProtocol(const parameterset* const instance_parameters);
      virtual void Init(const parameterset*const class_parameters, bool open_immediate=true) override;

      void set_read_timeout(int timeout_ms);
      int get_read_timeout() const;
      void set_write_timeout(int timeout_ms);
      int get_write_timeout() const;


    protected:
      int _vid;
      int _pid;
      int _epout;
      int _epin;
      int _write_timeout = 0;
      int _read_timeout = 0;
      std::unique_ptr<LibUsbDevice> _dev = nullptr;

    private:
      foxtrot::Logging _lg;
      LibUsbContext ctxt;

    };

  }
}
