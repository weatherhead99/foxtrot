#include <foxtrot/ft_tuple_helper.hh>



std::size_t foxtrot::tuple_size(const rttr::type& tp)
{
    auto sizeprop = tp.get_property("size");
    if(!sizeprop.is_valid())
        throw std::logic_error("size property isn't valid. Perhaps tuple type isn't registered");
    
    auto sizevar = tp.get_property_value("size");
    if(!sizevar.is_valid())
    {
        throw std::logic_error("tuple doesn't have a size. Perhaps it isn't registered");
    }
    
    
    const std::size_t& sz = sizevar.get_value<std::size_t>();
    
    
    return sz;
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
    auto tp = var.get_type();
    auto getmeth = tp.get_method("get");
    if(!getmeth.is_valid())
    {
        throw std::logic_error("tuple doesn't have a get method. Perhaps it isn't registered");
    }
    return getmeth.invoke(var,n);
}

