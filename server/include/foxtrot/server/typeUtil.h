#include <rttr/type>
#include <foxtrot/ft_types.pb.h>
#include <utility>

namespace foxtrot{
    class Logging;
    
    bool is_POD_struct(const rttr::type& tp, Logging* lg=nullptr);
    
    ft_simplevariant get_simple_variant_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    
    ft_struct get_struct_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    ft_enum get_enum_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    
    ft_variant get_variant_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    
    rttr::variant wire_type_to_variant(const ft_variant& wiretp, 
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    
    rttr::variant wire_type_to_variant(const ft_simplevariant& wiretp, 
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    rttr::variant wire_type_to_variant(const ft_struct& wiretp, 
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    rttr::variant wire_type_to_variant(const ft_enum& wiretp,
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    
    variant_descriptor describe_type(const rttr::type& tp, Logging* lg=nullptr);
    
    std::pair<simplevalue_types, unsigned char> describe_simple_type(const rttr::type& tp, Logging* lg=nullptr);
    struct_descriptor describe_struct(const rttr::type& tp, Logging* lg=nullptr);
    enum_descriptor describe_enum(const rttr::type& tp, Logging* lg=nullptr);
    
    
}
