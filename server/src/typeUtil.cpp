#include <foxtrot/server/typeUtil.h>


using namespace foxtrot;

bool is_POD_struct(const rttr::type& tp)
{
    //TODO: is an enum class counted as a class here?
    if(!tp.is_class() && !tp.is_enumeration())
        return false;
    
    unsigned meth_count = tp.get_methods().size();
    if(meth_count > 0)
        return false;

    unsigned constructor_count = tp.get_constructors().size();
    if(constructor_count > 0)
        return false;

    return true;
}


template<typename T, typename Tcall>
auto get_from_variant(const rttr::variant& var, Tcall callable, bool& success) ->
decltype( (std::declval<rttr::variant>().*callable)(std::declval<bool*>()), T())
{
    return (var.*callable)(&success);
};

template<typename T, typename Tcall>
auto get_from_variant(const rttr::variant& var, Tcall callable, bool& success) ->
decltype( (std::declval<rttr::variant>().*callable)(), T())
{
    success = true;
    return (var.*callable)();
}

struct variant_setter
{
    variant_setter(const rttr::variant& var, ft_simplevariant& out, bool& success)
    : _var(var), _out(out), _success(success) {};

    template<typename T, typename Tvarcall, typename Tmesscall>
    bool set_val(Tvarcall varcall, Tmesscall messcall)
    {
        auto tp = _var.get_type();
        if(tp == rttr::type::get<T>())
        {
            T val = get_from_variant<T>(_var,varcall, _success);
            (_out.*messcall)(val);
            return true;
        }
        return false;
    };
    
    template<typename T, typename Tvarcall, typename Tmesscall>
    bool set_check(Tvarcall varcall, Tmesscall messcall)
    {
        if(set_val<T>(varcall, messcall))
        {
            if(!_success)
            {
                auto tp = _var.get_type();
                throw std::logic_error("failed to convert type with name: " 
                + tp.get_name().to_string());
            }
            return true;
        }
        return false;
    };
    
    const rttr::variant& _var;
    ft_simplevariant& _out;
    bool& _success;
    
};


ft_simplevariant get_simple_variant_wire_type(const rttr::variant& var)
{
    ft_simplevariant out;
    
    bool success;
    
    if(!var.is_valid())
        throw std::logic_error("invalid RTTR variant supplied to get_simple_variant_wire_type");
    
    auto tp = var.get_type();
    if(!tp.is_valid())
        throw std::logic_error("invalid RTTR type from variant in get_simple_variant_wire_type");
    
    if(tp == rttr::type::get<void>())
        return out;
    
    if(tp.is_arithmetic())
    {
        using rttr::variant;
        variant_setter setter(var, out, success);
        if(setter.set_check<bool>(&variant::to_bool, &ft_simplevariant::set_boolval))
            return out;
        if(setter.set_check<double>(&variant::to_double, &ft_simplevariant::set_dblval)) 
            return out;
        if(setter.set_check<float>(&variant::to_float, &ft_simplevariant::set_dblval))
            return out;
        if(setter.set_check<int>(&variant::to_int, &ft_simplevariant::set_intval))
            return out;


    }
    
    
    
};


ft_struct get_struct_wire_type(const rttr::variant& var)
{
    ft_struct out;
    
    auto tp = var.get_type();
    out.set_struct_name(tp.get_name().to_string());
    
    auto prop_map = out.mutable_value();
    
    for(auto& prop : tp.get_properties())
    {
        
    }
};

