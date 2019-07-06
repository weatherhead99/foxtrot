#include <foxtrot/server/typeUtil.h>


using namespace foxtrot;

bool foxtrot::is_POD_struct(const rttr::type& tp)
{
    //TODO: is an enum class counted as a class here?
    if(!tp.is_class() && !tp.is_enumeration())
        return false;

    unsigned meth_count = tp.get_methods().size();
    if(meth_count > 0)
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


ft_simplevariant foxtrot::get_simple_variant_wire_type(const rttr::variant& var)
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
    

    
    using rttr::variant;
    variant_setter setter(var, out, success);
    if(tp.is_arithmetic())
    {
        if(setter.set_check<bool>(&variant::to_bool, &ft_simplevariant::set_boolval))
            return out;
        if(setter.set_check<double>(&variant::to_double, &ft_simplevariant::set_dblval)) 
            return out;
        if(setter.set_check<float>(&variant::to_float, &ft_simplevariant::set_dblval))
            return out;
        if(setter.set_check<int>(&variant::to_int, &ft_simplevariant::set_intval))
            return out;
        if(setter.set_check<short>(&variant::to_int16, &ft_simplevariant::set_intval))
            return out;
        if(setter.set_check<char>(&variant::to_int8, &ft_simplevariant::set_intval))
            return out;
        if(setter.set_check<unsigned>(&variant::to_uint32, &ft_simplevariant::set_uintval))
            return out;
        if(setter.set_check<unsigned short>(&variant::to_uint16, &ft_simplevariant::set_uintval))
            return out;
        if(setter.set_check<unsigned char>(&variant::to_uint8, &ft_simplevariant::set_uintval))
            return out;
    }
    else
    {
        if(var.get_type() == rttr::type::get<std::string>())
        {
            auto str = var.to_string(&success);
            if(!success)
                throw std::logic_error("failed to convert string value!");
            out.set_stringval(std::move(str));
            return out;
        }
        else
        {
            throw std::logic_error("couldn't find matching mapping for variant with type: " + var.get_type().get_name().to_string());
        }
    }
};


ft_struct foxtrot::get_struct_wire_type(const rttr::variant& var)
{
    ft_struct out;
    
    auto tp = var.get_type();
    out.set_struct_name(tp.get_name().to_string());
    
    auto prop_map = out.mutable_value();
    
    for(auto& prop : tp.get_properties())
    {
        //WARNING: this is a potentially infinite recursive call
        auto simplevar = get_variant_wire_type(prop.get_value(var));
        prop_map->operator[](prop.get_name().to_string()) = simplevar;
    }
    
    return out;
};

ft_enum foxtrot::get_enum_wire_type(const rttr::variant& var)
{
    ft_enum out;
    out.set_enum_name(var.get_type().get_name().to_string());

    auto enumvar = var.get_type().get_enumeration();

    auto outnames = out.mutable_enum_map();

    auto varit = enumvar.get_values().begin();
    
    bool ok = false;
    for(auto& name : enumvar.get_names())
    {
        outnames->operator[](name.to_string()) = (varit++)->to_uint32(&ok);
        if(!ok)
            throw std::logic_error("failed to convert an enum value");
    };

    out.set_enum_value(var.to_uint32(&ok));

    if(!ok)
        throw std::logic_error("failed to convert an enum value");
    
    return out;
};

ft_variant foxtrot::get_variant_wire_type(const rttr::variant& var)
{
    if(!var.is_valid())
        throw std::logic_error("got invalid variant to convert!");
    
    ft_variant out;
    
    auto tp = var.get_type();
    
    if(tp.is_enumeration())
    {
        ft_enum* enumval = out.mutable_enumval();
        *enumval = get_enum_wire_type(var);
    }
    else if(tp.is_class() && tp != rttr::type::get<std::string>())
    {
        if(!is_POD_struct(tp))
        {
            throw std::logic_error("can't return a class which isn't a POD type");
        }
        ft_struct* structval = out.mutable_structval();
        *structval = get_struct_wire_type(var);
    }
    else
    {
        ft_simplevariant* simplevarval = out.mutable_simplevar();
        *simplevarval = get_simple_variant_wire_type(var);
    }

    return out;
};

