#pragma once
#include <rttr/wrapper_mapper.h>
#include <optional>
#include <rttr/registration>
#include <foxtrot/concepts.hh>
#include <ft_types.pb.h>

namespace rttr
{
  template<typename T>
  struct wrapper_mapper<std::optional<T>>
  {
    using wrapped_type = typename std::optional<T>::value_type;
    using type = std::optional<T>;

    inline static wrapped_type get(const type& obj)
    {
      return obj.value();
    }

    inline static type create(const wrapped_type& value)
    {
      return std::optional<T>(value);
    }

    template<typename U>
    inline static std::optional<T> convert(const type& source, bool& ok)
    {

    }
    

  };

  
}

namespace foxtrot
{
  namespace detail
  {
    template<foxtrot::Optional T>
    bool has_value(const T& in)
    {
      return in.has_value();
    };

    template<foxtrot::Optional T>
    rttr::type optional_held_type()
    {
      using Val = typename T::value_type;
      return rttr::type::get<Val>();
    }

    template<foxtrot::Optional T>
    T::value_type optional_get(const T& in)
    {
      return in.value();
    }

    template<foxtrot::Optional T>
    T optional_converter(const typename T::value_type& in, bool& ok)
    {
      ok = true;
      return in;
    }
    
  }


  template<foxtrot::Optional T>
  void register_optional()
  {
    auto tp = rttr::type::get<T>();
    static auto nm = tp.get_name().to_string();
    rttr::registration::class_<T> reg(nm);

    reg(rttr::metadata("ft_type", variant_types::NULLABLE_TYPE))
      .constructor()(rttr::policy::ctor::as_object)
      .method("has_value", &detail::has_value<T>)
      .method("get", &detail::optional_get<T>)
      .property_readonly("held_type", &detail::optional_held_type<T>)
      ;

    rttr::type::register_converter_func(detail::optional_converter<T>);

  }

  rttr::type optional_held_type(const rttr::type& tp);
  bool optional_has_value(const rttr::variant& in);
  rttr::variant optional_get(const rttr::variant& in);

  
  
}

