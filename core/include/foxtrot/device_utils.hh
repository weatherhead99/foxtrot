#pragma once

#include <foxtrot/Device.h>
#include <foxtrot/Logging.h>
#include <variant>
#include <rttr/type>
#include <boost/hana/tuple.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/for_each.hpp>
#include <foxtrot/ReflectionError.h>
#include <string>
#include <optional>

using std::variant;
using std::string;
using std::optional;

namespace foxtrot
{
    namespace hana = boost::hana;
    
    namespace detail
    {
        template<typename Tuple>
        struct to_std_variant;
        
        template<typename... Ts>
        struct to_std_variant<hana::set<Ts...>>
        {
            using type = std::variant<typename Ts::type...>;
        };
        
        template<typename...Ts>
        using to_std_variant_t = typename to_std_variant<Ts...>::type;
        
    }
    
    constexpr auto ft_simplevar_types = boost::hana::make_set(
        hana::type_c<bool>, hana::type_c<unsigned char>, hana::type_c<char>,
        hana::type_c<unsigned short>, hana::type_c<short>, hana::type_c<unsigned>,
        hana::type_c<int>, hana::type_c<unsigned long>, hana::type_c<long>,
        hana::type_c<float>, hana::type_c<double>, hana::type_c<string>);
    
    
    using ft_simplevar_std_variant = typename detail::to_std_variant<std::decay_t<decltype(ft_simplevar_types)>>::type;
    
    template<typename StdVariant = ft_simplevar_std_variant>
    StdVariant rttr_to_std_variant_converter(const rttr::variant& var, Logging* lg= nullptr)
    {
     
        StdVariant out;
        bool done;
        
        boost::hana::for_each(ft_simplevar_types, 
                              [&out, &var, &done] (auto v) {
                                  using Type = typename decltype(+v)::type;

				  if(var.get_type().is_wrapper())
				    {
				      if(var.get_type().get_wrapped_type() == rttr::type::get<Type>())
					out = var.get_wrapped_value<Type>();
				      done = true;
				    }
                                  if( var.is_type<Type>())
				    {
                                      out = var.get_value<Type>();
                                      done = true;
                                  }
                              });
        if(!done)
            throw std::logic_error("failed to convert rttr variant to std::variant");
        return out;
    }
                                             
    
    
    
};
