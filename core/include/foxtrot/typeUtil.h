#pragma once

#include <rttr/type>
#include <foxtrot/ft_types.pb.h>
#include <utility>
#include <foxtrot/foxtrot_core_export.h>
#include <vector>
#include <foxtrot/HandleManager.hh>

namespace foxtrot{
    class Logging;
  
    
    FOXTROT_CORE_EXPORT std::vector<unsigned char> variant_to_bytes(const rttr::variant& var, bool check=false, Logging* lg=nullptr);
    
    FOXTROT_CORE_EXPORT rttr::variant bytes_to_variant(unsigned char* indata, const rttr::type& tgt_value_type, Logging* lg=nullptr);
    
    FOXTROT_CORE_EXPORT bool is_POD_struct(const rttr::type& tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT bool is_tuple(const rttr::type& tp, Logging* lg = nullptr);
    
    FOXTROT_CORE_EXPORT ft_simplevariant get_simple_variant_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    
    FOXTROT_CORE_EXPORT byte_data_types get_byte_data_type(const rttr::type& tp, Logging* lg=nullptr);

    FOXTROT_CORE_EXPORT ft_struct get_struct_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT ft_enum get_enum_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT ft_tuple get_tuple_wire_type(const rttr::variant& var, Logging* lg = nullptr);
    FOXTROT_CORE_EXPORT ft_homog_array get_array_wire_type(const rttr::variant& var, Logging* lg=nullptr);
    
    
    FOXTROT_CORE_EXPORT ft_variant get_variant_wire_type(const rttr::variant& var,
                                                         Logging* lg=nullptr,
							 );
    
    
    FOXTROT_CORE_EXPORT rttr::variant wire_type_to_variant(const ft_variant& wiretp, 
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    
    FOXTROT_CORE_EXPORT rttr::variant wire_type_to_variant(const ft_simplevariant& wiretp, 
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT rttr::variant wire_type_to_variant(const ft_struct& wiretp, 
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT rttr::variant wire_type_to_variant(const ft_enum& wiretp,
                                       const rttr::type& target_tp, Logging* lg=nullptr);
    
    FOXTROT_CORE_EXPORT rttr::variant wire_type_to_array(const ft_homog_array& wiretp, const rttr::type& target_tp, Logging* lg=nullptr);
    
    FOXTROT_CORE_EXPORT variant_descriptor describe_type(const rttr::type& tp, Logging* lg=nullptr);
    
    
    
    FOXTROT_CORE_EXPORT std::pair<simplevalue_types, unsigned char> describe_simple_type(const rttr::type& tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT struct_descriptor describe_struct(const rttr::type& tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT enum_descriptor describe_enum(const rttr::type& tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT tuple_descriptor describe_tuple(const rttr::type& tp, Logging* lg=nullptr);
    FOXTROT_CORE_EXPORT homog_array_descriptor describe_array(
        const rttr::type& tp, Logging* lg=nullptr);
    
}
