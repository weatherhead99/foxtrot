#pragma once
#include <google/protobuf/timestamp.pb.h>
#include <chrono>
#include <foxtrot/Logging.h>


namespace foxtrot
{
  template<typename Clock, typename Duration>
  google::protobuf::Timestamp to_protobuf_timestamp(const std::chrono::time_point<Clock, Duration>& tpt);
  

  template<typename TimePoint>
  TimePoint to_std_chrono(const google::protobuf::Timestamp& ts);
    
}



template <typename Clock, class Duration>
google::protobuf::Timestamp foxtrot::to_protobuf_timestamp(const std::chrono::time_point<Clock, Duration>& tpt)
{
  using google::protobuf::Timestamp;
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::seconds;
  using std::chrono::nanoseconds;

  //foxtrot::Logging lg("to_protobuf_timestamp");
  
  Timestamp out;
  
  using FloatNano = duration<double, nanoseconds::period>;

  auto since_epoch = tpt.time_since_epoch();
  auto seconds_part = duration_cast<seconds>(since_epoch);
  //lg.strm(sl::trace) << "seconds: " << seconds_part;
  //  lg.strm(sl::trace) << "seconds count: " << seconds_part.count();
  out.set_seconds(seconds_part.count());

  auto frac_part = since_epoch - seconds_part;
  auto nanos = duration_cast<nanoseconds>(frac_part);
  //lg.strm(sl::trace) << "nanoseconds: " << nanos;
  
  out.set_nanos(nanos.count());
  //lg.strm(sl::trace) << "debugstring: " << out.DebugString();

  return out;

}


template <typename TimePoint = std::chrono::high_resolution_clock::time_point>
TimePoint foxtrot::to_std_chrono(const google::protobuf::Timestamp& ts)
{
  auto secs = std::chrono::seconds(ts.seconds());
  auto nanos = std::chrono::nanoseconds(ts.nanos());

  return TimePoint(secs + nanos);
}
