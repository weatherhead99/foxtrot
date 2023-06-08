#include <foxtrot/typeUtil.h>
#include <foxtrot/Logging.h>
#include <foxtrot/ft_tuple_helper.hh>
#include <foxtrot/ReflectionError.h>

#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/ext/std/integral_constant.hpp>
#include <boost/hana/find.hpp>

#include <foxtrot/device_utils.hh>
#include <map>
#include <typeinfo>
#include <any>

using namespace foxtrot;


using boost::hana::pair;

using boost::hana::make_map;
using boost::hana::make_pair;
using boost::hana::type_c;

const auto bdt_map = make_map(
                    make_pair(type_c<unsigned char>, byte_data_types::UCHAR_TYPE),
                    make_pair(type_c<char>, byte_data_types::CHAR_TYPE),
                    make_pair(type_c<unsigned short>, byte_data_types::USHORT_TYPE),
                    make_pair(type_c<unsigned>, byte_data_types::UINT_TYPE),
                    make_pair(type_c<unsigned long>, byte_data_types::ULONG_TYPE),
                    make_pair(type_c<short>, byte_data_types::USHORT_TYPE),
                    make_pair(type_c<int>, byte_data_types::IINT_TYPE),
                    make_pair(type_c<long>, byte_data_types::LONG_TYPE),
                    make_pair(type_c<float>, byte_data_types::BFLOAT_TYPE),
                    make_pair(type_c<double>, byte_data_types::BDOUBLE_TYPE)
                              );

template<typename T>
using Sizeof = std::integral_constant<unsigned char, sizeof(T)>;

    

std::vector<unsigned char> foxtrot::variant_to_bytes(const rttr::variant& var, bool check, Logging* lg)
{
    if(lg)
        lg->strm(sl::debug) << "in new variant_to_bytes function" ;
    if(check)
    {
        if(!var.get_type().is_sequential_container())
            throw std::logic_error("variant is not a sequential container, can't turn it into bytes!!!");
    }
    
    auto view = var.create_sequential_view();
    
    if(check)
        if(!view.get_rank() == 1)
            throw std::logic_error("can only deal with rank 1 arrays for now!");
        
    
    std::size_t nbytes = view.get_value_type().get_sizeof() / sizeof(unsigned char) * view.get_size();
    
    if(lg)
        lg->strm(sl::trace) << "nbytes: " << nbytes;
    
    bool done = false;
    std::vector<unsigned char> out;
    out.resize(nbytes);
    
    boost::hana::for_each(bdt_map, [&view, &done, &out, nbytes, lg] (auto v) {
        if(done)
            return;
       using K = typename decltype(+hana::first(v))::type;
       auto value_type = view.get_value_type();
       if(rttr::type::get<K>() == value_type)
       {
           if(lg)
               lg->strm(sl::debug) << "matched value type: " << value_type.get_name().to_string();
           std::vector<K> intermediate;
           intermediate.reserve(view.get_size());
        
           std::optional<bool> is_wrapped;
           
           for(auto& item : view)
           {   
               //TODO: not sure if this works for non-wrapped values as well
               if(!is_wrapped.has_value())
                   *is_wrapped = item.get_type().is_wrapper();
               
               if(*is_wrapped)
                   intermediate.push_back(item.get_wrapped_value<K>());
               else
                   intermediate.push_back(item.get_value<K>());
               
//                if(lg)
//                    lg->strm(sl::trace) << "value type trace: " << item.get_type().get_name().to_string();
           }
           if(lg)
           {
               lg->strm(sl::debug) << "intermediate array is populated";
               lg->strm(sl::debug) << "intermediate size: " << intermediate.size();
               
               std::ostringstream oss;
               for(auto v : intermediate)
                   oss << std::dec << (int) v  << ",";
               
               lg->strm(sl::trace) << "array values: " << oss.str();
               
               oss.str("");
               for(auto item: view)
                   oss << item.to_string() << ",";
               lg->strm(sl::trace) << "orig values: " << oss.str();
               
                   
           }
           
           auto* ptr = reinterpret_cast<unsigned char*>(intermediate.data());
           std::copy(ptr, ptr+nbytes, out.begin());
           done = true;
           
           if(lg)
               lg->strm(sl::trace) << "copy to unsigned char vector complete";
       }
    });
    
    if(!done)
        throw std::logic_error("failed to convert array to bytes");
    return out;   
}


rttr::variant foxtrot::bytes_to_variant(unsigned char* indata, const rttr::type& tgt_value_type, Logging* lg)
{
    rttr::variant out = tgt_value_type.create();
    bool done;
    boost::hana::for_each(bdt_map, [indata, &done, &tgt_value_type, &out] (auto v) {
        using K = typename decltype(+boost::hana::first(v))::type;
        if(done)
            return;
        if(rttr::type::get<K>() == tgt_value_type)
        {
            K intermediate = *( reinterpret_cast<K*>(indata));
            out = intermediate;
        }
    });
    
    if(!done)
        throw std::logic_error("failed to convert bytes to variant!");
    
    std::advance(indata, tgt_value_type.get_sizeof());
    return out;
}

bool foxtrot::is_POD_struct(const rttr::type& tp, Logging* lg)
{
    //TODO: is an enum class counted as a class here?
    if(!tp.is_class() && !tp.is_enumeration())
        return false;

    auto meth_count = tp.get_methods().size();
    if(meth_count > 0)
        return false;

    auto constructor_count = tp.get_constructors().size();
    if(constructor_count == 0)
        return false;
    
    return true;
}

bool foxtrot::is_tuple(const rttr::type& tp, Logging* lg)
{
    if(!tp.get_metadata("tuplemeta").is_valid())
    {
        if(lg)
            lg->strm(sl::debug) << "no tuplemeta";
        return false;
    }
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

#if 0
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

#endif


ft_simplevariant foxtrot::get_simple_variant_wire_type(const rttr::variant& var, Logging* lg)
{
    ft_simplevariant out;
    
    bool success;
    auto tp = var.get_type();
    if(!tp.is_valid())
        throw std::logic_error("invalid RTTR type from variant in get_simple_variant_wire_type");
    
    if(tp == rttr::type::get<void>())
    {
        out.set_is_void(true);
        return out;
    }
    
    if(!var.is_valid())
    {
        throw std::logic_error("simple_variant got invalid input!");
    }
    out.set_is_void(false);
          
    
    auto stdvariant = rttr_to_std_variant_converter(var, lg);
    if(stdvariant.index() == std::variant_npos)
        throw std::logic_error("couldn't find matching mapping for variant with type: " +   var.get_type().get_name().to_string());

    //this visitor should get all the arithmetic and string types...
    
    bool done = false;
    
    if(lg)
        lg->strm(sl::debug) << "variant index: "<< (int) stdvariant.index();
    
    std::visit([&out, lg, & done] (auto && v) {
        namespace hana = boost::hana;
        
        const auto dblval_types = hana::make_set( hana::type_c<float>,
                                                    hana::type_c<double>);
        
        const auto intval_types = hana::make_set(hana::type_c<int>,
                                                    hana::type_c<short>,
                                                    hana::type_c<char>,
                                                    hana::type_c<long>
                                        );
        
        const auto uintval_types = hana::make_set(hana::type_c<unsigned>,
                                                    hana::type_c<unsigned short>,
                                                    hana::type_c<unsigned char>,
                                                    hana::type_c<unsigned long>);
        
    
        //NOTE: the std::decay_t thing here is CRUCIALLY important!
        using Vtype = std::decay_t<decltype(v)>;
        auto VT = hana::type_c<Vtype>;
        
        out.set_size(sizeof(Vtype));
        
        if(lg)
            lg->strm(sl::debug) << typeid(Vtype).name() ;
        
        if constexpr(hana::contains(dblval_types, VT))
        {
            if(lg)
                lg->strm(sl::trace) << "stdvariant dblval" ;
            out.set_dblval(v);
            done =true;
        }
        else if constexpr(VT == hana::type_c<bool>)
        {
            if(lg)
                lg->strm(sl::trace) << "stdvariant boolval";
            out.set_boolval(v);
            done = true;
        }
        else if constexpr(hana::contains(intval_types, VT) == hana::true_())
        {
            if(lg)
                lg->strm(sl::trace) << "stdvariant intval";
            out.set_intval(v);
            done = true;
        }
        else if constexpr(hana::contains(uintval_types, VT) == hana::true_())
        {
            if(lg)
                lg->strm(sl::trace) << "stdvariant uintval";
            out.set_uintval(v);
            done = true;
        }
        else if constexpr(VT == hana::type_c<string>)
        {
            if(lg)
                lg->strm(sl::trace) << "stdvariant string value";
            out.set_stringval(v);
            done = true;
        }
    }, stdvariant);
    
    if(!done)
        throw std::logic_error("invalid control flow in simplevalue conversion ,shouldn't ever happen!");
    return out;
};


byte_data_types foxtrot::get_byte_data_type(const rttr::type& tp, Logging* lg)
{
    byte_data_types out;
    bool done = false;
    
    boost::hana::for_each(bdt_map,
                          [&tp, &out, &done] (auto v) {
                             
                              if(done)
                                  return;
                              
                              //K is the type of the variant we're matching on
                               using K = typename decltype(+hana::first(v))::type;
                               auto rttr_cmp = rttr::type::get<K>();
                               if(tp == rttr_cmp)
                               {
                                   out = hana::second(v);
                                   done = true;
                               }
                          });
    
    if(!done)
    {
        if(lg)
            lg->strm(sl::error) << "failed getting BDT from rttr type with name: " << tp.get_name().to_string();
        throw std::logic_error("couldn't get byte data type");
    }
    
    return out;
}

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

ft_homog_array foxtrot::get_array_wire_type(const rttr::variant& var,
                                   Logging* lg)
{
    auto view = var.create_sequential_view();
    if(view.get_rank() > 1)
    {
        if(lg)
            lg->strm(sl::error) << "array rank is: " << view.get_rank();
            throw ReflectionError("unable to deal with arrays of greater than rank 1");
    }
    
    auto contained_type = view.get_rank_type(1);
    
    byte_data_types bdt;
    try 
    {
        bdt = get_byte_data_type(contained_type, lg);
    }
    catch(std::logic_error& err)
    {
        if(lg)
            lg->strm(sl::error) << "contained type was not a simple variant when trying to map array!";
        throw(err);
    }
    
    ft_homog_array_encoded encout;
    encout.set_dtp(bdt);
    
    if(lg)
        lg->strm(sl::trace) << "copying byte data to return type";
    
    auto tmpdat = variant_to_bytes(var, false, lg);
    std::string tmpstr{tmpdat.begin(), tmpdat.end()};
    
    auto* dat = encout.mutable_data();
    *dat = tmpstr;
    
    if(lg)
        lg->strm(sl::trace) << "byte data copy completed";
    
    
    ft_homog_array out;
    *out.mutable_arr_encoded() = std::move(encout);
    
    return out;
}


ft_variant foxtrot::get_variant_wire_type(const rttr::variant& var,
                                          Logging* lg)
{
    if(lg)
    {
        lg->strm(sl::trace) << "var is valid? " << var.is_valid();
        lg->strm(sl::trace) << "var type: " << var.get_type().get_name();
    }
    
    ft_variant out;
    auto tp = var.get_type();
    if(!tp.is_valid())
        throw std::logic_error("got invalid RTTR type, this is a foxtrot bug");
    
    if(tp == rttr::type::get<void>())
    {
        ft_simplevariant* simplevar = out.mutable_simplevar();
        simplevar->set_is_void(true);
        return out;
    }
    if(!var.is_valid())
    {
            throw std::logic_error("get_variant_wire_type: got invalid variant to convert!");
    }

    if(tp.is_enumeration())
    {
        if(lg)
            lg->strm(sl::trace) << "enumeration logic";
        ft_enum* enumval = out.mutable_enumval();
        *enumval = get_enum_wire_type(var, lg);
    }
    else if(tp.is_sequential_container())
    {
        if(lg)
            lg->strm(sl::trace) << "array logic";
        auto* arrayval = out.mutable_arrayval();
        *arrayval = get_array_wire_type(var, lg);
        
        if(lg)
            lg->strm(sl::trace) << "wrote output array definition";
        
    }
    else if(tp.is_class() && tp != rttr::type::get<std::string>() && is_POD_struct(tp))
    {
        if(lg)
            lg->strm(sl::trace) << "struct logic";
        ft_struct* structval = out.mutable_structval();
        *structval = get_struct_wire_type(var, lg);
    }
    else if(is_tuple(tp,lg))
    {
        if(lg)
            lg->strm(sl::trace) << "tuple logic";
        ft_tuple* tupleval = out.mutable_tupleval();
        *tupleval = get_tuple_wire_type(var, lg);
    }
    else if(tp.is_class() && tp != rttr::type::get<std::string>())
    {
        std::string err_msg = "dont understand how to convert type: " + tp.get_name().to_string();
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
    else if(target_tp.is_sequential_container())
    {
        if(lg)
            lg->strm(sl::trace) << "checking array case";
        if(wiretp.value_case() != ft_variant::ValueCase::kArrayval)
            throw std::runtime_error("expected array, got something else in supplied type");
        return wire_type_to_array(wiretp.arrayval(), target_tp, lg);
        
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

rttr::variant foxtrot::wire_type_to_array(const ft_homog_array& wiretp, const rttr::type& target_tp, Logging* lg)
{
    if(wiretp.has_arr_encoded())
        throw std::logic_error("can't deal with decoded arrays at the moment!");
    
    auto encarr = wiretp.arr_encoded();
    
    rttr::variant out = target_tp.create();
    
    if(!target_tp.is_sequential_container())
        throw std::logic_error("tried to create an array at non target array type");
    
    auto view = out.create_sequential_view();
    
    if(view.is_dynamic())
        view.set_size(encarr.data().size());
    else
        if(view.get_size() != encarr.data().size())
            throw ReflectionError("invalid array size passed");

    if(view.get_rank() != 1)
        throw std::logic_error("can only handle arrays of rank 1 at present!");
    auto value_type = view.get_rank_type(1);
    
    auto bdt = get_byte_data_type(value_type, lg);
    if(bdt != encarr.dtp())
    {
        if(lg)
        {
            lg->strm(sl::error) << "target value type: " << value_type.get_name().to_string();
            lg->strm(sl::error) << "byte data type: " << (int) bdt;
        throw ReflectionError("invalid byte data type passed!");
        }  
    }
    
    auto szof = value_type.get_sizeof();    
    auto* stringit = reinterpret_cast<unsigned char*>(encarr.mutable_data()->data());
    for(auto tgt_item : view)
    {
        auto tgtvar = bytes_to_variant(stringit, value_type, lg);
    }
    
    return out;
    
}


variant_descriptor foxtrot::describe_type(const rttr::type& tp, foxtrot::Logging* lg)
{
    variant_descriptor out;
    out.set_cpp_type_name(tp.get_name().to_string());
    
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

homog_array_descriptor describe_array(const rttr::type& tp,
                                      Logging* lg)
{
    
    
    homog_array_descriptor out;
    //for now, all arrays are "simple"
    out.set_is_simple(true);
    //for now, all arrays are "encoded"
    out.set_is_encoded(true);
    //and they are all not fixed size
    out.set_has_fixed_size(false);
    
    return out;
}



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
    decltype(tp.get_sizeof()) size = 0;
    if(!tp.is_arithmetic())
    {   
        if(tp == rttr::type::get<void>())
            out = simplevalue_types::VOID_TYPE;
        else if(tp == rttr::type::get<std::string>())
            out =  simplevalue_types::STRING_TYPE;
	else if(tp == rttr::type::get<std::any>() or tp.is_wrapper() or tp.is_pointer())
	  out = simplevalue_types::REMOTE_HANDLE_TYPE;
	
    }
    else
    {
        if(lg)
            lg->strm(sl::trace) << "type is arithmetic";
        if(tp == rttr::type::get<bool>())
            out = simplevalue_types::BOOL_TYPE;
        else if(is_type_any_of<float,double>(tp))
            out =  simplevalue_types::FLOAT_TYPE;
        else if(is_type_any_of<char, short, int, long, long long>(tp))
        {
            if(lg)
                lg->strm(sl::trace) << "type is INT";
            out = simplevalue_types::INT_TYPE;
        }
        else if(is_type_any_of<unsigned char, unsigned short, unsigned, unsigned long, unsigned long long>(tp))
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


