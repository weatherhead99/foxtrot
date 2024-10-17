#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>


int main(int argc, char** argv)
{
  const std::string teststr = "abcdef";
  
  auto checksum = std::accumulate(teststr.begin(), teststr.end(), 0, 
    [] (const char& c1, const char& c2)
    {
      return static_cast<int>(c1) + static_cast<int>(c2);
    }
  );
  
  std::ostringstream oss;
  oss << std::setw(3) << std::setfill('0')<<  checksum << '\r';
  
  std::cout << oss.str() << std::endl;
  
  
  int sum = 0;
  for (auto& c : teststr)
  {
    std::cout <<"char: " << c << "   integer: " << static_cast<int>(c)<< std::endl; ;
     sum += static_cast<int>(c);
  }
  std::cout << "total sum: " << sum << std::endl;;
  std::cout << "modulo 256: " << sum % 256 << std::endl;
  
  
  


}