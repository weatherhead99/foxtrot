#include <foxtrot/ft_optional_helper.hh>
#include <rttr/type>

rttr::type foxtrot::optional_held_type(const rttr::type& tp)
{
  auto tpprop = tp.get_property("held_type");
  if(!tpprop.is_valid())
    throw std::logic_error("held_type property isn't valid, Perhaps optional type isn't registered");

  if(!tpprop.is_static())
    throw std::logic_error("held_type property isn't static. Perhaps optional type isn't registered");

  auto ret = tpprop.get_value(rttr::instance());

  if(ret.get_type() != rttr::type::get<rttr::type>())
    throw std::logic_error("optional held_type didn't return a type!");

  return ret.get_value<rttr::type>();

}

bool foxtrot::optional_has_value(const rttr::variant& in)
{
  auto hasvalprop = in.get_type().get_method("has_value");

  if(!hasvalprop.is_valid())
    {

      throw std::logic_error("optional doesn't have a valid has_value method");
    }

  if(!hasvalprop.is_static())
    throw std::logic_error("optional has_value wasn't static. Registration issue");
 
  auto ret =  hasvalprop.invoke(rttr::instance(), in);

  if(ret.is_type<bool>())
    return ret.get_value<bool>();

  throw std::logic_error("optional has_value returned the wrong type!");
}

rttr::variant foxtrot::optional_get(const rttr::variant& in)
{
  auto getmeth = in.get_type().get_method("get");

  if(!getmeth.is_valid())
    throw std::logic_error("optional get method not valid. Registration issue");
  return getmeth.invoke(rttr::instance(), in);
  
}
  
