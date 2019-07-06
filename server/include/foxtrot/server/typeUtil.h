#include <rttr/type>
#include <foxtrot/ft_types.pb.h>


namespace foxtrot{

    bool is_POD_struct(const rttr::type& tp);
    
    ft_simplevariant get_simple_variant_wire_type(const rttr::variant& var);
    
    ft_struct get_struct_wire_type(const rttr::variant& var);
    
}
