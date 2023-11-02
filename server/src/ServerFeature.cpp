#include "ServerFeature.hh"

namespace foxtrot
{

  std::size_t ServerFeatureBase::next_id() noexcept
  {
    static std::size_t counter = 0;
    return counter++;
  }


  
}


