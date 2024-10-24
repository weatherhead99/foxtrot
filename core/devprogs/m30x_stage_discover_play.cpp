#include <iostream>
#include <vector>
#include <string>
#include <foxtrot/udev/foxtrot_udev_utils.hh>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;

using DeviceIdentifier = std::string;

std::vector<DeviceIdentifier> find_possible_system_devices()
{
  foxtrot::udev_context ctxt;
  auto qry = foxtrot::udev_enum(ctxt).match_subsystem("tty").match_property("ID_VENDOR_ID", "0403");

  std::vector<DeviceIdentifier> out;
  
  for(const auto& [name, _] : qry.scan_devices())
    {
      foxtrot::udev_device dev(ctxt, name);
      cout << "ID_USB_MODEL_ID: " << dev.property()["ID_USB_MODEL_ID"] << endl;
      cout << "DEVNAME: " << dev.property()["DEVNAME"] << endl;
    }
  
  
  return {};
}



int main()
{
  find_possible_system_devices();
  

}
