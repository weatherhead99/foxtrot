#include <foxtrot/protocols/libUsbProtocol.hh>
#include <libusb.h>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>
#include <foxtrot/protocols/ProtocolUtilities.h>
#include <iostream>


using namespace foxtrot::protocols;


std::string format_libusb_err_msg(int errcode)
{
  std::ostringstream oss;
  oss << "libUSB error: " << libusb_error_name(errcode) << ": " << libusb_strerror(errcode);
  return oss.str();
}

auto check_libusb_return(auto ret, foxtrot::Logging* lg=nullptr)
{
  //NOTE: all libusb error codes are <0, this allows passthrough
  if(ret < 0)
    {
      if(lg  != nullptr)
	throw LibUsbError(ret, *lg);
      throw LibUsbError(ret);
    }
  return ret;
}


LibUsbError::LibUsbError(int errcode)
  : ProtocolError(format_libusb_err_msg(errcode))
{
  code = errcode;
};

LibUsbError::LibUsbError(int errcode, foxtrot::Logging& lg)
  : ProtocolError(format_libusb_err_msg(errcode), lg)
{
  code = errcode;
};


LibUsbContext::LibUsbContext()
{
  check_libusb_return(libusb_init(&_ptr));
};

LibUsbContext::~LibUsbContext()
{
  if(_ptr != nullptr)
    libusb_exit(_ptr);
}

libusb_context* LibUsbContext::ptr()
{
  return _ptr;
}



void LibUsbDeviceList::_LibUsbDeviceListDeleter::operator()(libusb_device** list)
{
  //NOTE: unref the devices, if any other LibUsbDevice objects have been created
  // they upref themselves on copy / construction
  libusb_free_device_list(list, true);
}


LibUsbDeviceList::const_iterator::const_iterator(const LibUsbDeviceList *devlist,
						 int pos)
    : _devlist(devlist), _pos(pos) {}

LibUsbDevice LibUsbDeviceList::const_iterator::operator*()
{  
  return _devlist->operator[](_pos);
}

foxtrot::protocols::LibUsbDeviceList::const_iterator&
LibUsbDeviceList::const_iterator::operator++()
{
  _pos++;
  return *this;
}

foxtrot::protocols::LibUsbDeviceList::const_iterator
LibUsbDeviceList::const_iterator::operator++(int)
{
  return const_iterator(_devlist, _pos+1);

}




LibUsbDeviceList::LibUsbDeviceList(LibUsbContext& ctxt)
{


  //allocate and get a device list. The machinery of unique_ptr custom deleter should free it when we're done
  libusb_device** pdevlist;
  _n_devices = check_libusb_return(libusb_get_device_list(ctxt.ptr(), &pdevlist));
  devlist = decltype(devlist)(pdevlist);

};

LibUsbDeviceList::~LibUsbDeviceList()
{
}


int LibUsbDeviceList::n_devices() const
{
  return _n_devices;
}

LibUsbDevice LibUsbDeviceList::operator[](std::size_t pos) const
{

  
  if(pos >= _n_devices)
    throw std::out_of_range("illegal device list index");

  LibUsbDevice out(*(devlist.get() + pos));
  return out;

}

LibUsbDeviceList::const_iterator LibUsbDeviceList::cbegin() const
{
  return const_iterator(this, 0);
}

LibUsbDeviceList::const_iterator LibUsbDeviceList::begin() const
{
  return cbegin();
}

LibUsbDeviceList::const_iterator LibUsbDeviceList::cend() const
{
  return const_iterator(this, n_devices());
}

LibUsbDeviceList::const_iterator LibUsbDeviceList::end() const
{
  return cend();
}


LibUsbDevice::LibUsbDevice(libusb_device* pdev): _devptr(pdev)
{
  libusb_ref_device(pdev);
}



bool foxtrot::protocols::operator==(const LibUsbDeviceList::const_iterator& a, const LibUsbDeviceList::const_iterator& b)
{ 
  return (a._pos == b._pos) and ( a._devlist == b._devlist);
}

bool foxtrot::protocols::operator!=(const LibUsbDeviceList::const_iterator& a, const LibUsbDeviceList::const_iterator& b)
{
  return not (a == b);
}


libusb_device_descriptor LibUsbDevice::device_descriptor() const
{
  libusb_device_descriptor out;
  check_libusb_return(libusb_get_device_descriptor(_devptr,&out));
  return out;
}

LibUsbDevice::~LibUsbDevice()
{
  if(is_open())
    close();

  if(_devptr != nullptr)
    libusb_unref_device(_devptr);
}

LibUsbDevice::LibUsbDevice(const LibUsbDevice& other)
{
  _devptr = other._devptr;
  libusb_ref_device(_devptr);
}

LibUsbDevice::LibUsbDevice(LibUsbDevice&& other)
{
  _devptr = other._devptr;
  _hdl =  other._hdl;

  other._devptr = nullptr;
  other._hdl = nullptr;

  if(other.claimed_interface.has_value())
    *claimed_interface = *other.claimed_interface;

  other.claimed_interface.reset();
}


bool LibUsbDevice::is_open() const
{
  return _hdl != nullptr;
}

void LibUsbDevice::open()
{
  
  
  check_libusb_return(libusb_open(_devptr, &_hdl));
}

void LibUsbDevice::set_configuration(int configuration)
{
  if(not is_open())
    throw std::logic_error("tried to set configuration on closed device!");

  check_libusb_return(libusb_set_configuration(_hdl, configuration));
}

int LibUsbDevice::configuration()
{
  if(not is_open())
    throw std::logic_error("tried to get configuration on closed device!");

  int cfg_out = 0;

  check_libusb_return(libusb_get_configuration(_hdl, &cfg_out));
  return cfg_out;
}

void LibUsbDevice::clear_halt(unsigned short endpoint)
{
  if(not is_open())
    throw std::logic_error("tried to set configuration on closed device!");

  check_libusb_return(libusb_clear_halt(_hdl, endpoint));
}

void LibUsbDevice::close() noexcept
{

  
  
  libusb_close(_hdl);
  _hdl = nullptr;
}

void LibUsbDevice::reset()
{
  if(not is_open())
    throw std::logic_error("tried to set reset a  device!");
  check_libusb_return(libusb_reset_device(_hdl));
}

void LibUsbDevice::claim_interface(std::uint8_t iface)
{
  if(claimed_interface != std::nullopt)
    throw std::logic_error("an interface is already claimed!");
  check_libusb_return(libusb_claim_interface(_hdl, iface));
  claimed_interface = iface;
}

void LibUsbDevice::release_interface()
{
  if(not claimed_interface)
    throw std::logic_error("no interface is claimed!");

  check_libusb_return(libusb_release_interface(_hdl, *claimed_interface));
  claimed_interface = std::nullopt;
}

std::vector<unsigned char> LibUsbDevice::blocking_control_transfer_receive(std::uint8_t bmRequestType,
								 std::uint8_t bRequest,
								 std::uint16_t wValue,
								 std::uint16_t wIndex,
								 std::uint16_t wLength,
								 std::chrono::milliseconds timeout)
{

  std::vector <unsigned char> out;
  out.resize(wLength);
  check_libusb_return(libusb_control_transfer(_hdl, bmRequestType,
					      bRequest, wValue, wIndex, out.data(),
					      wLength, timeout.count()));

  return out;
}


void LibUsbDevice::blocking_control_transfer_send(std::uint8_t bmRequestType,
						  std::uint8_t bRequest,
						  std::uint16_t wValue,
						  std::uint16_t wIndex,
						  const std::span<unsigned char> data,
						  std::chrono::milliseconds timeout)
{
  check_libusb_return(libusb_control_transfer(_hdl, bmRequestType, bRequest,
					      wValue, wIndex, data.data(),
					      data.size(), timeout.count()));


}


std::vector<unsigned char> LibUsbDevice::blocking_bulk_transfer_receive(unsigned char endpoint,
									int maxlen, std::chrono::milliseconds timeout)
{
  std::vector<unsigned char> out;
  out.resize(maxlen);

  int transferred = 0;
  check_libusb_return(libusb_bulk_transfer(_hdl, endpoint, out.data(), maxlen, &transferred, timeout.count()));

  out.resize(transferred);
  return out;
}

void LibUsbDevice::blocking_bulk_transfer_send(unsigned char endpoint,
					       const std::span<unsigned char> data,
					       std::chrono::milliseconds timeout,
					       int* transferred)
{
  check_libusb_return(
		      libusb_bulk_transfer(_hdl, endpoint, data.data(), data.size(),
					   transferred, timeout.count())
		      );

}



std::optional<LibUsbDevice> foxtrot::protocols::find_single_device(LibUsbContext& ctxt, unsigned short vid, unsigned short pid, bool throw_on_multi)
{
  LibUsbDeviceList devlist(ctxt);
  auto match_vidpid = [vid, pid] (const auto& dev)
  {
    auto desc = dev.device_descriptor();
    return (desc.idVendor == vid) and (desc.idProduct == pid);
  };

  auto devit = std::find_if(devlist.cbegin(), devlist.cend(), match_vidpid);
  if(devit == devlist.cend())
    {
      return std::nullopt;
    }

  if(throw_on_multi)
    {
      //now we need to check for a 2nd device
      auto devit_cpy = devit;
      devit_cpy = std::find_if(++devit_cpy, devlist.cend(), match_vidpid);
      if(devit_cpy != devlist.cend())
	throw std::runtime_error("multiple matching devices found");
    }

  return *devit;

}


libUsbProtocol::libUsbProtocol(const parameterset *const instance_parameters)
    : SerialProtocol(instance_parameters), _lg("libUsbProtocol"){

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

    _lg.Info("searching attached USB devices...");
    auto found_dev = find_single_device(ctxt, _vid, _pid);

    if(not found_dev.has_value())
      throw ProtocolError("failed to find appropriate device!");

    try
      {
	_lg.strm(sl::debug) << "opening device...";
	(*found_dev).open();
	_lg.strm(sl::debug) << "resetting device..";
	(*found_dev).reset();
	_lg.strm(sl::debug) << "setting configuration...";
	(*found_dev).set_configuration(1);
	_lg.strm(sl::debug) << "claiming interface...";
	(*found_dev).claim_interface(0);
      }
    catch(foxtrot::protocols::LibUsbError& err)
      {
	std::ostringstream oss;
	oss << "libusb error: " << err.what();
	throw foxtrot::ProtocolError(oss.str());
      }

    _dev = std::make_unique<LibUsbDevice>(std::move(*found_dev));

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
