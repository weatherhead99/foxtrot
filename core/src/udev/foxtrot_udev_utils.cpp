#include <foxtrot/udev/foxtrot_udev_utils.hh>
#include <string>
#include <sstream>

using namespace foxtrot;

udev_enum::udev_enum(udev_context& ctxt)
  : _udev_enum_rc(ctxt.get())
{
  


};

udev_enum& udev_enum::match_subsystem(const string& expr)
{

  auto ret = udev_enumerate_add_match_subsystem(_udev_enum_rc.get(), expr.c_str());
  if(ret < 0)
    {
      std::ostringstream oss;
      oss << "udev error, code: " << ret;
     
      throw std::runtime_error(oss.str());
    }

			       
  
  
  return *this;
};
