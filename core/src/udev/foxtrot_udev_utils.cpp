#include <foxtrot/udev/foxtrot_udev_utils.hh>
#include <string>
#include <sstream>

#include <iostream>
using std::cout;
using std::endl;

using namespace foxtrot;

udev_enum::udev_enum(udev_context& ctxt)
  : _udev_enum_rc(ctxt.get())
{
  


};

udev_enum& udev_enum::match_subsystem(const string& expr)
{

  auto ret = udev_enumerate_add_match_subsystem(_udev_enum_rc.get(), expr.c_str());
  check_throw_udev_return(ret);
  return *this;
};

udev_enum& udev_enum::match_property(const string& propname, const string& propvalue)
{
  auto ret = udev_enumerate_add_match_property(_udev_enum_rc.get(), propname.c_str(),
					      propvalue.c_str());
  check_throw_udev_return(ret);
  return *this;
}


udev_list udev_enum::scan_devices()
{
  
  auto ret = udev_enumerate_scan_devices(_udev_enum_rc.get());
  check_throw_udev_return(ret);


  auto* ptr = udev_enumerate_get_list_entry(_udev_enum_rc.get());

  udev_list out;
  out._start_ptr = ptr;

  return out;
}

void udev_enum::check_throw_udev_return(int ret)
{
  if(ret < 0)
    {
      std::ostringstream oss;
      oss << "udev error, code: " << ret;
     
      throw std::runtime_error(oss.str());
    }      
}


udev_list_iterator& udev_list_iterator::operator++()
{
  _ptr =  udev_list_entry_get_next(_ptr);
  return *this;
}

udev_list_iterator udev_list_iterator::operator++(int)
{
  auto cp = *this;
  ++(*this);
  return cp;
}

udevListEntry udev_list_iterator::operator*()
{
  auto name = udev_list_entry_get_name(_ptr);
  const char* val = udev_list_entry_get_value(_ptr);

  if(val != nullptr)
    return {name, std::string(val)};

  return {name, std::nullopt};
}

udev_list_iterator udev_list::begin()
{
  return udev_list_iterator(_start_ptr);
}

udev_list_iterator udev_list::end()
{
  return udev_list_iterator(nullptr);
}


udev_list_iterator::udev_list_iterator(udev_list_entry* ptr): _ptr(ptr)
{}

udev_list_iterator::udev_list_iterator(const udev_list_iterator& other)
{
  _ptr = other._ptr;
}


bool foxtrot::operator==(const udev_list_iterator& a, const udev_list_iterator& b)
{
  return a._ptr == b._ptr;
}

bool foxtrot::operator!=(const udev_list_iterator& a, const udev_list_iterator& b)
{
  return a._ptr != b._ptr;
}


foxtrot::udev_device::udev_device(udev_context& ctxt, const std::string& syspath)
{
  auto* udevptr = udev_device_new_from_syspath(ctxt.get(),syspath.c_str());
  _dev = udev_device_rc(udevptr, false);
};

udev_list foxtrot::udev_device::properties()
{
  udev_list out;
  out._start_ptr = udev_device_get_properties_list_entry(_dev.get());
  
  return out;
}

foxtrot::detail::_udev_map_read_interface_proxy foxtrot::udev_device::property()
{
  return foxtrot::detail::_udev_map_read_interface_proxy(udev_device_get_property_value, *this);
}
