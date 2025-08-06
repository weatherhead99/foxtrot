#include <foxtrot/ft_union_helper.hh>



rttr::method load_check_method(const rttr::variant& var, const std::string& methname)
{
  auto tp = var.get_type();

  if(tp.is_wrapper())
    tp = tp.get_wrapped_type();
  auto meth = tp.get_method(methname);

  if(!meth.is_valid())
    throw std::logic_error("invalid method on union type, foxtrot programming error");

  return meth;
}


rttr::variant foxtrot::union_get(const rttr::variant& var)
{
  auto getmeth = load_check_method(var, "get");

  //TODO: slow as balls
  if(var.get_type().is_wrapper())
    {
      auto inner_union = var.extract_wrapped_value();
      return getmeth.invoke(rttr::instance(), inner_union);
    }
  
  return getmeth.invoke(rttr::instance(), var);

}

std::vector<rttr::type> foxtrot::union_possible_types(const rttr::variant& var)
{
  auto meth = load_check_method(var, "possible_types");
  auto postypes = meth.invoke(rttr::instance(), var).get_value<std::vector<rttr::type>>();

  return postypes;
}


rttr::type foxtrot::union_held_type(const rttr::variant& var)
{
  auto meth = load_check_method(var, "held_type");
  auto outtp = meth.invoke(rttr::instance(), var).get_value<rttr::type>();
  return outtp;
};
