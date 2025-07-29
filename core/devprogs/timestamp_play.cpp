#include <iostream>
#include <foxtrot/timestamp_utils.hh>
#include <foxtrot/ft_timestamp_helper.hh>
#include <google/protobuf/text_format.h>
#include <chrono>
#include <rttr/type>
#include <foxtrot/typeUtil.h>
#include <foxtrot/Logging.h>

using std::cout;
using std::endl;

int main()
{


  using FloatNano = std::chrono::duration<double, std::chrono::nanoseconds::period>;
  
  auto now = std::chrono::high_resolution_clock::now();
    
  auto pbts = foxtrot::to_protobuf_timestamp(now);


  std::string s;
  google::protobuf::TextFormat::PrintToString(pbts, &s);
  std::cout << "timestamp: " << s << std::endl;
  auto nowcpy = foxtrot::to_std_chrono<std::chrono::high_resolution_clock::time_point>(pbts);
  auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(now - nowcpy);
  
  std::cout << "converted diff (nanoseconds): " << diff.count() << std::endl;

  std::cout << "converted back: " << nowcpy << std::endl;

  auto tp = rttr::type::get<decltype(now)>();
  
  cout << "type name:" << tp.get_name();

  rttr::variant rtvar = now;

  foxtrot::Logging lg("timestamp_play");
  foxtrot::setLogFilterLevel(sl::trace);

  auto pbts_test = foxtrot::get_variant_wire_type(rtvar,
						  &lg);

  cout << "timestamp debug string: " << pbts_test.DebugString() << endl;
}


RTTR_REGISTRATION
{
  using TStamp = std::chrono::time_point<std::chrono::high_resolution_clock>;
  foxtrot::register_timestamp<TStamp>();

}
