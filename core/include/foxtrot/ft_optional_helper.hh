#pragma once
#include <boost/hana/concept/product.hpp>
#include <rttr/wrapper_mapper.h>
#include <optional>
#include <rttr/registration>


namespace rttr
{
  template<typename T>
  struct wrapper_mapper<std::optional<T>>
  {
    using wrapped_type = std::optional<T>::value_type;
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

}

