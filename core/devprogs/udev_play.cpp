#include <iostream>
#include <libudev.h>
#include <string>

#include <foxtrot/udev/foxtrot_udev_utils.hh>

using std::cout;
using std::endl;

int main()
{
  foxtrot::udev_context ctxt;
  auto qry = foxtrot::udev_enum(ctxt).match_subsystem("block");


}



int other()
{
  auto* udev_ctxt = udev_new();


  auto udev_enum = udev_enumerate_new(udev_ctxt);
  auto n_subsystems = udev_enumerate_scan_subsystems(udev_enum);

  cout << "n_subsystems: " << n_subsystems << endl;
  
  udev_enumerate_add_match_subsystem(udev_enum, "block");
  n_subsystems = udev_enumerate_scan_subsystems(udev_enum);
  udev_enumerate_scan_devices(udev_enum);


  auto* udev_list_entry = udev_enumerate_get_list_entry(udev_enum);


  int i=0;
  while(udev_list_entry != nullptr)
    {
      std::string name = udev_list_entry_get_name(udev_list_entry);
      cout << "i:" << (i++) << " name: " << name <<   endl;

      auto* udev_dev = udev_device_new_from_syspath(udev_ctxt, name.c_str());

      std::string type = udev_device_get_devtype(udev_dev);
      std::string sysname = udev_device_get_sysname(udev_dev);

      cout << "type: " << type << " sysname: " << sysname << endl;

      
      udev_list_entry = udev_list_entry_get_next(udev_list_entry);


      

    }

  return 0;
  
}
