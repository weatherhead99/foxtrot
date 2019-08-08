#pragma once
#include <rttr/registration>

namespace foxtrot
{
    namespace detail
    {
 
    template<typename T, int N>
    struct tuple_helper
    {
        static rttr::variant get(const T& in, int n)
        {
            if(n == N)
            {
                return std::get<N>(in);
            }
            return tuple_helper<T, N-1>::get(in,n);
        };
        
        static rttr::type type(int n)
        {
            if(n == N)
            {
                using elemT = typename std::tuple_element<N,T>::type;
                return rttr::type::get<elemT>();
            }
            return tuple_helper<T, N-1>::type(n);
        }

    };

    template<typename T>
    struct tuple_helper<T,-1>
    {
        static rttr::variant get(const T&, int n) {f();};
        static rttr::type type(int n) {f();};
        static void f()
        {
            throw std::logic_error("invalid tuple index");
        }
        
    };
    

    template<typename T1, typename T2, int N>
    struct tuple_helper<std::pair<T1,T2>,N>
    {
        using Pair = std::pair<T1,T2>;
        static rttr::variant get(const Pair& in, int n)
        {
            if(n ==1)
                return in.second;
            if(n == 0)
                return in.first;
            
            throw std::logic_error("invalid tuple index");
        }

        static rttr::type type(int n)
        {
            if(n ==1)
                return rttr::type::get<T2>();
            if(n == 0)
                return rttr::type::get<T1>();
            
            throw std::logic_error("invalid tuple index");
        }
    };
    
    
    template<typename T>
    struct tuple_size_helper
    {
        enum { value = std::tuple_size<T>::value};
    };
    
    template<typename T1, typename T2>
    struct tuple_size_helper<std::pair<T1,T2>>
    {
        enum {value  = 2};
    };
    
    }
    
template<typename T>
rttr::variant tuple_get(const T& in, int n)
{
    constexpr int N = detail::tuple_size_helper<T>::value;
    return detail::tuple_helper<T,N-1>::get(in,n);
}
template<typename T>
rttr::type tuple_element_type(int n)
{
    constexpr int N = detail::tuple_size_helper<T>::value;
    return detail::tuple_helper<T,N-1>::type(n);
}

template<typename T>
std::size_t tuple_size()
{
    return detail::tuple_size_helper<T>::value;
}


template<typename T>
void register_tuple()
{
    auto tp = rttr::type::get<T>();
    static auto nm = tp.get_name().to_string();
    rttr::registration::class_<T> reg(nm);
    reg(rttr::metadata("tuplemeta",true))
    .constructor()
    .method("get",&tuple_get<T>)
    .property_readonly("size",&tuple_size<T>)
    .method("type",&tuple_element_type<T>);
};


rttr::type tuple_element_type(const rttr::type& tp, int n);
std::size_t tuple_size(const rttr::type& tp);
rttr::variant tuple_get(const rttr::variant& var, int n);

}
