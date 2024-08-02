#include <foxtrot/ft_tuple_helper.hh>

std::size_t foxtrot::tuple_size(const rttr::type& tp)
{
    auto sizeprop = tp.get_property("size");
    if(!sizeprop.is_valid())
        throw std::logic_error("size property isn't valid. Perhaps tuple type isn't registered");
    if(!sizeprop.is_static())
        throw std::logic_error("size property isn't static. Perhaps tuple type isn't registered");
    
    rttr::variant sizeret = sizeprop.get_value(rttr::instance());
    
    if(sizeret.is_type<std::size_t>())
    {
        const std::size_t& sz = sizeret.get_value<std::size_t>();
        return sz;
    }
    
    throw std::logic_error("size property returned the wrong type. Perhaps tuple type isn't registered");
    
}

rttr::type foxtrot::tuple_element_type(const rttr::type& tp, int n)
{
    auto typemeth = tp.get_method("type");
    if(!typemeth.is_valid())
    {
        throw std::logic_error("tuple doesn't have a type method. Perhaps it isn't registered");
    }
    auto elem_type_ret = typemeth.invoke(rttr::instance(), {n});
    
    if(elem_type_ret.get_type() != rttr::type::get<rttr::type>())
    {
        throw std::logic_error("type of return isn't an rttr::type... this is invalid");
    }
    
    return elem_type_ret.get_value<rttr::type>();
}

rttr::variant foxtrot::tuple_get(const rttr::variant& var, int n)
{
  bool wrapper = false;
    auto tp = var.get_type();
    if(tp.is_wrapper())
      {
	wrapper = true;
	tp = tp.get_wrapped_type();
      }
    
    auto getmeth = tp.get_method("get");
    if(!getmeth.is_valid())
    {
        throw std::logic_error("tuple doesn't have a get method. Perhaps it isn't registered");
    }

    if(not wrapper)
      return  getmeth.invoke(rttr::instance(), var, n);

    //NOTE: unfortunate copy....
    return getmeth.invoke(rttr::instance(), var.extract_wrapped_value(), n);
}

