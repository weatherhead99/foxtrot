#include <memory>
#include <iostream>
#include <string>

extern "C" {

  void function1(int arg1)
  {
    std::cout << arg1 << std::endl;
  }

  std::string function2(const std::string& in)
  {
    std::cout << in << std::endl;
    return "hello";
  }
  
  
  
}


std::string function3(const std::string& in)
{

  std::cout << in << std::endl;
  return "hello2";
}
