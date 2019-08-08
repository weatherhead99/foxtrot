#pragma once
#include <rttr/registration>

namespace foxtrot
{
    namespace detail
    {
        
    template<typename T, int N>
    struct tuple_getter_helper
    {
        static rttr::variant get(const T& in, int n)
        {
            if(n == N)
            {
                return std::get<N>(in);
            }
            return tuple_getter_helper<T, N-1>::get(in,n);
        };
    };

    template<typename T>
    struct tuple_getter_helper<T,-1>
    {
        static rttr::variant get(const T&, int n)
        {
            throw std::logic_error("invalid tuple index");
        }
    };
    
    template<typename T, int N>
    struct tuple_type_helper
    {
        static rttr::type type(int n)
        {
            if(n == N)
            {
                using elemT = typename std::tuple_element<N,T>::type;
                return rttr::type::get<elemT>();
            }
            return tuple_type_helper<T, N-1>::type(n);
        };
    };
    
    template<typename T>
    struct tuple_type_helper<T,-1>
    {
        static rttr::type type(int n)
        {
            throw std::logic_error("invalid tuple index");
        }
    };
    
    
    }
    
template<typename T>
rttr::variant tuple_get(const T& in, int n)
{
    constexpr int N = std::tuple_size<T>::value;
    return detail::tuple_getter_helper<T,N-1>::get(in,n);
}
template<typename T>
rttr::type tuple_element_type(int n)
{
    constexpr int N = std::tuple_size<T>::value;
    return detail::tuple_type_helper<T,N-1>::type(n);
}

template<typename T>
std::size_t tuple_size()
{
    return std::tuple_size<T>::value;
}


template<typename T>
void register_tuple()
{
    auto tp = rttr::type::get<T>();
    static auto nm = tp.get_name().to_string();
    rttr::registration::class_<T>(nm)
    .constructor()
    .method("get",&tuple_get<T>)
    .property_readonly("size",&tuple_size<T>)
    .method("type",&tuple_element_type<T>);
};


rttr::type tuple_element_type(const rttr::type& tp, int n);
std::size_t tuple_size(const rttr::type& tp);
rttr::variant tuple_get(const rttr::variant& var, int n);

}
