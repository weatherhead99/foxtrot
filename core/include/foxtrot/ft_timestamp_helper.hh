#pragma once

#include <cinttypes>
#include <rttr/type>
#include <rttr/registration>
#include <ft_types.pb.h>
#include <foxtrot/timestamp_utils.hh>

namespace foxtrot {

  template<typename T>
  concept StdTimePoint = requires(T t)
  {
    { t.time_since_epoch() };
    
  };

  
  namespace detail
  {
    template<StdTimePoint T>
    google::protobuf::Timestamp pb_tstamp_helper(const T& in)
    {
      using Clock = T::clock;
      using Duration = T::duration;
      return foxtrot::to_protobuf_timestamp<Clock, Duration>(in);
    }
  }
  

  
  template <foxtrot::StdTimePoint T>
  void register_timestamp()
  {
    using Clock = T::clock;
    using Duration = T::duration;
    
    auto dyntp = rttr::type::get<T>();
    static auto nm = dyntp.get_name().to_string();
    rttr::registration::class_<T> reg(nm);
    reg(rttr::metadata("ft_type", variant_types::SIMPLEVAR_TYPE),
	rttr::metadata("timestampmeta", true))
      .constructor()(rttr::policy::ctor::as_object)
      .method("to_protobuf_timestamp", &detail::pb_tstamp_helper<T>)
      ;
    

  }


  
}



