#include "ServerFeature.hh"

namespace foxtrot
{

  std::size_t ServerFeatureBase::next_id() noexcept
  {
    static std::size_t counter = 0;
    return counter++;
  }


  std::size_t ServerFeature::feat_type_id() noexcept
  {
    static std::size_t typeid_ = ServerFeatureBase::next_id();
    return typeid_;
  }
  
}


