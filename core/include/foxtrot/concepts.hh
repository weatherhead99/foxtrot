#pragma once

namespace foxtrot
{
  template <typename T>
    concept Optional = requires(const T& t)
    {
      { t.has_value()};
      { t.operator*()};
    };

}
