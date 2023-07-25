#include <foxtrot/ft_union_helper.hh>

rttr::variant foxtrot::union_get(const rttr::variant& var)
{
  auto tp = var.get_type();
  auto getmeth = tp.get_method("get");

  if(!getmeth.is_valid())
    throw std::logic_error("invalid get method on union type, foxtrot programming error!");

  return getmeth.invoke(rttr::instance(), var);

}
