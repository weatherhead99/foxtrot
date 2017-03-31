#include "devices/newport_2936R/newport2936R.h"
#include <iostream>
#include <rttr/type>
#include <rttr/variant.h>


using namespace foxtrot::devices;
using std::cout;
using std::endl;


int main(int argc, char** argv)
{
  auto unit = powerunits::Watts_cm2;
  
  rttr::type::register_converter_func(convert_powerunit_to_string);
  rttr::variant v(unit);

  cout << "type: " << v.get_type().get_name() << endl;
  
  cout << " type is valid: " << v.get_type().is_valid() << endl;
  
  cout << "can convert to string: " << v.can_convert(rttr::type::get<std::string>()) << endl;
  
  auto ok = v.convert(rttr::type::get<std::string>());
  cout << "ok? : " << (int) ok << endl;
  
  cout << v.to_string() << endl;
  
  cout << v.get_value<std::string>() << endl;
  
  rttr::variant v2("A");
  
  cout << "type: " <<  v2.get_type().get_name() << endl;
  
  auto units = rttr::type::get<foxtrot::devices::powerunits>();
  
  cout << "can convert to powerunits: " << v.can_convert(units) << endl;

  
  auto unitout = v.get_value<foxtrot::devices::powerunits>();
  
    cout << std::to_string((unsigned short) unitout) << endl;
    
  
  
  
  
}