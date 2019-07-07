#include <rttr/type>
#include <foxtrot/ft_types.pb.h>


namespace foxtrot{

    bool is_POD_struct(const rttr::type& tp);
    
    ft_simplevariant get_simple_variant_wire_type(const rttr::variant& var);
    
    ft_struct get_struct_wire_type(const rttr::variant& var);
    ft_enum get_enum_wire_type(const rttr::variant& var);
    
    ft_variant get_variant_wire_type(const rttr::variant& var);
    
    rttr::variant wire_type_to_variant(const ft_variant& wiretp);
    
    rttr::variant wire_type_to_variant(const ft_simplevariant& wiretp);
    rttr::variant wire_type_to_variant(const ft_struct& wiretp);
    rttr::variant wire_type_to_variant(const ft_enum& wiretp);
    
}
