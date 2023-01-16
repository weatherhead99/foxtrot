#include "hwfind_utils.hh"
#include <memory>

#ifdef __linux__
#include <libudev.h>

#endif



using foxtrot::devices::hw_serial_port_handle;



namespace foxtrot {

  namespace devices {
#ifdef __linux__
    
    vector<hw_serial_port_handle> find_usb_serial_port(optional<unsigned int> idVendor,
						       optional<unsigned int> idProduct,
						       optional<unsigned int> iSerial)
    {
      
      
    }
#else
    vector<hw_serial_port_handle> find_usb_serial_port(optional<unsigned int> idVendor,
						       optional<unsigned int> idProduct,
						       optional<unsigned int> iSerial)
    {
      throw std::runtime_error("finding serial ports not supported yet on non-linux platforms!");
    }

#endif
  }

}



