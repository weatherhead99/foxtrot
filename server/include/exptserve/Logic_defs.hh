#pragma once
#include <boost/function_types/parameter_types.hpp>
#include <boost/mpl/at.hpp>
#include <type_traits>
#include <stdexcept>
#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/Logging.h>

namespace foxtrot
{
    
    namespace detail
    {
        template<typename F, unsigned char n> 
        struct get_function_parameter_type
        {
            using type = typename std::remove_pointer<
                typename boost::mpl::at<
                typename boost::function_types::parameter_types<F>,
                typename std::integral_constant<unsigned char, n>>::type>::type;
            
        };
        
        template<typename T>
        struct extract_value_type
        {
            using value_type = T;
        };
        
        template<template<typename> class X, typename T>
        struct extract_value_type<X<T>>
        {
            using value_type = T;
        };
        
        template<typename> struct member_function_traits;
        
        template<typename Return, typename Object, typename... Args>
        struct member_function_traits<Return (Object::*)(Args...)>
        {
            using instance_type = Object;
        };
        
        
    }
    
    class HandlerTag;
    
    
    template<auto Fun, typename Service, typename F=decltype(Fun)>
    struct Serverlogic_defs
    {
        //second argument to F, pointer removed
        using reqtp = typename detail::get_function_parameter_type<F,2>::type;
        //third argument to F, pointer removed
        using respondertp = typename detail::get_function_parameter_type<F,3>::type;
        using repltp = typename detail::extract_value_type<respondertp>::value_type;
        
        using servicetp = Service;
        
        constexpr static auto requestfunptr = Fun;
        
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
        {
            //default implementation throw a not implemented error 
            foxtrot::Logging lg("Serverlogic_defs");
            foxtrot_server_specific_error("method not implemented yet.", repl,
                                          respond,lg,error_types::ft_ServerError);
            return true;
            
        }
        
        bool check_metadata(grpc::ServerContext& _ctxt, reqtp& req)
        {
            return true;
        }
        
    };
    
};
