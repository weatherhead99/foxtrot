#pragma once

#include <vector>
#include <string>
#include <optional>

namespace foxtrot
{
  namespace devices
  {
    using std::vector;
    using std::string;
    using std::optional;

#ifdef __linux__
    struct hw_serial_port_handle
    {
      string sysfs_path;
    };
#else
    struct hw_serial_port_handle
    {
    };
#endif

    vector<hw_serial_port_handle> find_usb_serial_port(optional<unsigned int> idVendor = std::nullopt,
						       optional<unsigned int> idProduct = std::nullopt,
						       optional<unsigned int> iSerial = std::nullopt);

    
    

  }
}
