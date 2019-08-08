#include <foxtrot/typeUtil.h>
#include <foxtrot/Logging.h>
#include <foxtrot/ft_tuple_helper.hh>

using namespace foxtrot;

bool foxtrot::is_POD_struct(const rttr::type& tp, Logging* lg)
{
    //TODO: is an enum class counted as a class here?
    if(!tp.is_class() && !tp.is_enumeration())
        return false;

    unsigned meth_count = tp.get_methods().size();
    if(meth_count > 0)
        return false;

    unsigned constructor_count = tp.get_constructors().size();
    if(constructor_count == 0)
        return false;
    
    return true;
}

bool foxtrot::is_tuple(const rttr::type& tp, Logging* lg)
{
    if(!tp.is_template_instantiation())
    {
        if(lg)
            lg->strm(sl::trace) << "type is not template instantiation";
        return false;
    }
    auto tpname = tp.get_name().to_string();
    auto pos = tpname.find("std::tuple");
    if(lg)
        lg->strm(sl::debug) << "checking for std::tuple location" ;
    if(pos == std::string::npos)
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


ft_simplevariant foxtrot::get_simple_variant_wire_type(const rttr::variant& var, Logging* lg)
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
        out.set_size(tp.get_sizeof());
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


ft_struct foxtrot::get_struct_wire_type(const rttr::variant& var, Logging* lg)
{
    if(lg)
        lg->strm(sl::trace) << "in get_struct_wire_type";
    
    ft_struct out;
    
    auto tp = var.get_type();
    out.set_struct_name(tp.get_name().to_string());
    
    auto prop_map = out.mutable_value();
    
    for(auto& prop : tp.get_properties())
    {
        //WARNING: this is a potentially infinite recursive call
        auto simplevar = get_variant_wire_type(prop.get_value(var), lg);
        prop_map->operator[](prop.get_name().to_string()) = simplevar;
    }
    
    return out;
};

ft_enum foxtrot::get_enum_wire_type(const rttr::variant& var, Logging* lg)
{
    ft_enum out;
    auto enum_desc = out.mutable_desc();
    *enum_desc = describe_enum(var.get_type(),lg);
    
    bool ok;
    out.set_enum_value(var.to_uint32(&ok));

    if(!ok)
        throw std::logic_error("failed to convert an enum value");
    
    return out;
};

ft_tuple foxtrot::get_tuple_wire_type(const rttr::variant& var, Logging* lg)
{
    ft_tuple out;
    if(lg)
        lg->strm(sl::trace) << "getting tuple wire type";
    
    //WARNING: does not work yet!!!
    auto sz = tuple_size(var.get_type());
    if(lg)
        lg->strm(sl::trace) << "tuple size: " << sz;
    for(int i=0; i < sz; i++)
    {
        rttr::variant element_value = foxtrot::tuple_get(var, i);
        ft_variant* varpt = out.add_value();
        *varpt = get_variant_wire_type(element_value,lg);
    }
    
    return out;
}

ft_variant foxtrot::get_variant_wire_type(const rttr::variant& var, Logging* lg)
{
    if(lg)
    {
        lg->strm(sl::trace) << "var is valid? " << var.is_valid();
        lg->strm(sl::trace) << "var type: " << var.get_type().get_name();
    }
    
    if(!var.is_valid())
        throw std::logic_error("get_variant_wire_type: got invalid variant to convert!");
    
    ft_variant out;
    
    auto tp = var.get_type();
    
    if(tp.is_enumeration())
    {
        if(lg)
            lg->strm(sl::trace) << "enumeration logic";
        ft_enum* enumval = out.mutable_enumval();
        *enumval = get_enum_wire_type(var, lg);
    }
    else if(tp.is_class() && tp != rttr::type::get<std::string>() && is_POD_struct(tp))
    {
        if(lg)
            lg->strm(sl::trace) << "struct logic";
        ft_struct* structval = out.mutable_structval();
        *structval = get_struct_wire_type(var, lg);
    }
    else if(is_tuple(tp))
    {
        if(lg)
            lg->strm(sl::trace) << "tuple logic";
        ft_tuple* tupleval = out.mutable_tupleval();
        *tupleval = get_tuple_wire_type(var, lg);
    }
    else if(tp.is_class() && tp != rttr::type::get<std::string>())
    {
        static std::string err_msg = "dont understand how to convert type: " + tp.get_name().to_string();
        throw std::logic_error(err_msg);
    }
    else
    {
        if(lg)
            lg->strm(sl::trace) << "simplevalue logic";
        ft_simplevariant* simplevarval = out.mutable_simplevar();
        *simplevarval = get_simple_variant_wire_type(var, lg);
    }

    return out;
};


rttr::variant foxtrot::wire_type_to_variant(const ft_enum& wiretp, 
                                            const rttr::type& target_tp, Logging* lg)
{
    rttr::variant out = wiretp.enum_value();
    if(! out.can_convert(target_tp))
    {
        throw std::runtime_error("can't convert received type: " + out.get_type().get_name().to_string() + 
        "to target type: " + target_tp.get_name().to_string());
    }
    
    if(!out.convert(target_tp))
    {
        throw std::runtime_error("conversion is possible, but failed for some reason");
    }
    
    return out;
};

rttr::variant foxtrot::wire_type_to_variant(const ft_struct& wiretp,
                                            const rttr::type& target_tp, Logging* lg)
{
    rttr::variant out = target_tp.create();
    
    if(lg)
    {
        lg->strm(sl::debug) << "is type valid? " << target_tp.is_valid();
        lg->strm(sl::debug) << "is created variant valid: " << out.is_valid();
        lg->strm(sl::debug) << "type of created variant: " << out.get_type().get_name().to_string();
    }
    
    for(auto& prop : target_tp.get_properties())
    {
        auto invar = wiretp.value().at(prop.get_name().to_string());
        rttr::variant in_variant = wire_type_to_variant(invar, prop.get_type(), lg);
        if(lg)
        {
            lg->strm(sl::debug) << "is in_variant valid? " << in_variant.is_valid();
            
            lg->strm(sl::debug) << "is prop valid?" << prop.is_valid();
            
        }
        prop.set_value(out,in_variant);
    };

    return out;
}

rttr::variant foxtrot::wire_type_to_variant(const ft_simplevariant& wiretp,
                                            const rttr::type& target_tp, Logging* lg)
{
    rttr::variant out;
    switch(wiretp.value_case())
    {
        case(ft_simplevariant::ValueCase::kBoolval):
            out = wiretp.boolval();
            break;
        case(ft_simplevariant::ValueCase::kIntval):
            out = wiretp.intval();
            break;
        case(ft_simplevariant::ValueCase::kUintval):
            out = wiretp.uintval();
            break;
        case(ft_simplevariant::ValueCase::kDblval):
            out = wiretp.dblval();
            break;
        case(ft_simplevariant::ValueCase::kStringval):
            out = wiretp.stringval();
            break;
    }
    
    if(!out.can_convert(target_tp))
        throw std::runtime_error("cannot convert wire type received: " + out.get_type().get_name().to_string() + " to target type: " + target_tp.get_name().to_string());

    bool success = out.convert(target_tp);
    if(!success)
        throw std::runtime_error("type conversion is possible, but failed somehow");
    
    return out;
}

rttr::variant foxtrot::wire_type_to_variant(const ft_variant& wiretp,
                                            const rttr::type& target_tp, Logging* lg)
{
    if(lg)
    {
        lg->strm(sl::debug) << "target type: " << target_tp.get_name().to_string();
    }
    
    if(target_tp.is_enumeration())
    {
        if(wiretp.value_case() != ft_variant::ValueCase::kEnumval)
            throw std::runtime_error("expected variant, got something else in supplied type");

        return wire_type_to_variant(wiretp.enumval(),target_tp, lg);
    }
    else if(target_tp.is_class() && target_tp != rttr::type::get<std::string>())
    {
        if(lg)
            lg->strm(sl::trace) << "checking struct case";
        
        if(wiretp.value_case() != ft_variant::ValueCase::kStructval)
            throw std::runtime_error("expected struct, got something else in supplied type");
        return wire_type_to_variant(wiretp.structval(), target_tp, lg);
    }
    else
    {
        if(wiretp.value_case() != ft_variant::ValueCase::kSimplevar)
            throw std::runtime_error("expected simple var, got something else in supplied type");
        return wire_type_to_variant(wiretp.simplevar(), target_tp, lg);
    }

}

variant_descriptor foxtrot::describe_type(const rttr::type& tp, foxtrot::Logging* lg)
{
    variant_descriptor out;
    if(tp.is_enumeration())
    {
        if(lg)
            lg->strm(sl::debug) << "type is enumeration";
        auto* desc = out.mutable_enum_desc();
        *desc = describe_enum(tp);
        out.set_variant_type(variant_types::ENUM_TYPE);
    }
    else if(tp.is_class() && tp != rttr::type::get<std::string>() 
            && is_POD_struct(tp))
    {
        if(lg)
            lg->strm(sl::debug) << "type is struct";
        auto* desc = out.mutable_struct_desc();
        *desc = describe_struct(tp);
        out.set_variant_type(variant_types::STRUCT_TYPE);
    }
    else if(is_tuple(tp,lg))
    {
        if(lg)
            lg->strm(sl::trace) << "type is tuple";
        auto* desc = out.mutable_tuple_desc();
        *desc = describe_tuple(tp);
        out.set_variant_type(variant_types::TUPLE_TYPE);
    }
    else
    {
        if(lg)
            lg->strm(sl::debug) << "type is simple";
        auto desc = describe_simple_type(tp,lg);
        out.set_simplevalue_sizeof(desc.second);
        out.set_simplevalue_type(desc.first);
        out.set_variant_type(variant_types::SIMPLEVAR_TYPE);
    }
    
    return out;
}

struct_descriptor foxtrot::describe_struct(const rttr::type& tp, Logging* lg)
{
    struct_descriptor out;
    out.set_struct_name(tp.get_name().to_string());

    auto prop_map = out.mutable_struct_map();
    for(auto& prop : tp.get_properties())
    {
        auto var = describe_type(prop.get_type());
        prop_map->operator[](prop.get_name().to_string()) = var;
    }

    return out;
};


enum_descriptor foxtrot::describe_enum(const rttr::type& tp, Logging* lg)
{
    enum_descriptor out;
    out.set_enum_name(tp.get_name().to_string());
    
    auto outnames = out.mutable_enum_map();
    
    auto enumtp = tp.get_enumeration();
    auto enumvarit = enumtp.get_values().begin();
    
    for(auto& name : enumtp.get_names())
    {
        outnames->operator[](name.to_string()) = (enumvarit++)->to_uint32();
    };
    
    return out;
};


template<typename First, typename... Ts> 
struct is_type_any_of_impl
{
    static bool compare(const rttr::type& tp)
    {
        if(tp == rttr::type::get<First>())
            return true;
        return is_type_any_of_impl<Ts...>::compare(tp);
    };
    
};

template<typename Last> 
struct is_type_any_of_impl<Last>{
    static bool compare(const rttr::type& tp)
    {
        return tp == rttr::type::get<Last>();
    };
    
};

template<typename... Ts>
bool is_type_any_of(const rttr::type& tp)
{
    return is_type_any_of_impl<Ts...>::compare(tp);
}


std::pair<simplevalue_types,unsigned char> foxtrot::describe_simple_type(const rttr::type& tp, Logging* lg)
{
    simplevalue_types out;
    unsigned char size = 0;
    if(!tp.is_arithmetic())
    {   
        if(tp == rttr::type::get<void>())
            out = simplevalue_types::VOID_TYPE;
        else if(tp == rttr::type::get<std::string>())
            out =  simplevalue_types::STRING_TYPE;
    }
    else
    {
        if(lg)
            lg->strm(sl::trace) << "type is arithmetic";
        if(tp == rttr::type::get<bool>())
            out = simplevalue_types::BOOL_TYPE;
        else if(is_type_any_of<float,double>(tp))
            out =  simplevalue_types::FLOAT_TYPE;
        else if(is_type_any_of<char, short, int, long>(tp))
        {
            if(lg)
                lg->strm(sl::trace) << "type is INT";
            out = simplevalue_types::INT_TYPE;
        }
        else if(is_type_any_of<unsigned char, unsigned short, unsigned, unsigned long>(tp))
            out =  simplevalue_types::UNSIGNED_TYPE;
        else
        {
            throw std::logic_error("can't deduce appropriate descriptor for type: " + tp.get_name().to_string());
        }
        size = tp.get_sizeof();
    }
    return std::make_pair(out,size);
};

tuple_descriptor foxtrot::describe_tuple(const rttr::type& tp, Logging* lg)
{
    tuple_descriptor out;

    auto sz = foxtrot::tuple_size(tp);
    
    for(int i=0; i < sz; i++)
    {
        rttr::type element_type = foxtrot::tuple_element_type(tp,i);
        if(lg)
            lg->strm(sl::trace) << "element_type name: " << element_type.get_name();
        variant_descriptor* desc = out.add_tuple_map();
        *desc = foxtrot::describe_type(element_type, lg);
    }
    
    return out;
};
