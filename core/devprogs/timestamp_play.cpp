#include <iostream>
#include <foxtrot/timestamp_utils.hh>
#include <google/protobuf/text_format.h>
#include <chrono>


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
  

}
