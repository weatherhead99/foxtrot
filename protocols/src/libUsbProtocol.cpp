#include <foxtrot/protocols/libUsbProtocol.hh>
#include <libusb.h>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>
#include <foxtrot/protocols/ProtocolUtilities.h>

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


void LibUsbDeviceList::_LibUsbDeviceListDeleter::operator()(libusb_device** list)
{
  //NOTE: unref the devices, if any other LibUsbDevice objects have been created
  // they upref themselves on copy / construction
  libusb_free_device_list(list, true);
}


LibUsbDeviceList::const_iterator::const_iterator(LibUsbDeviceList *devlist,
                                                 int pos)
    : _devlist(devlist), _pos(pos) {}

LibUsbDevice LibUsbDeviceList::const_iterator::operator*()
{
  return _devlist->operator[](_pos);
}

LibUsbDeviceList::const_iterator& LibUsbDeviceList::const_iterator::operator++()
{
  _pos++;
  return *this;
}

LibUsbDeviceList::const_iterator LibUsbDeviceList::const_iterator::operator++(int)
{
  return const_iterator(_devlist, _pos+1);
  
}




LibUsbDeviceList::LibUsbDeviceList()
{

  //NOTE: this is deprecated upstream but only in newer libusb versions than we're using yet
  check_libusb_return(libusb_init(&_ctxt));


  //allocate and get a device list. The machinery of unique_ptr custom deleter should free it when we're done
  libusb_device** pdevlist;
  _n_devices = check_libusb_return(libusb_get_device_list(_ctxt, &pdevlist));
  devlist = decltype(devlist)(pdevlist);

};

LibUsbDeviceList::~LibUsbDeviceList()
{
   libusb_exit(_ctxt);
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

LibUsbDevice::const_iterator LibUsbDevice::cbegin() const
{
  return LibUsbDevice::const_iterator(this, 0);
}

LibUsbDevice::const_iterator LibUsbDevice::begin() const
{
  return cbegin();
}



LibUsbDevice::LibUsbDevice(libusb_device* pdev): _devptr(pdev)
{
  libusb_ref_device(pdev);
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

  libusb_unref_device(_devptr);
}

bool LibUsbDevice::is_open() const
{
  return _hdl == nullptr;
}

void LibUsbDevice::open()
{
  check_libusb_return(libusb_open(_devptr, &_hdl));
}

void LibUsbDevice::close() noexcept
{
  libusb_close(_hdl);
  _hdl = nullptr;
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
						  std::span<unsigned char> data,
						  std::chrono::milliseconds timeout)
{
  check_libusb_return(libusb_control_transfer(_hdl, bmRequestType, bRequest,
					      wValue, wIndex, data.data(),
					      data.size(), timeout.count()));
  

}
					       



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



