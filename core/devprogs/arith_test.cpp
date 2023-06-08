#include <rttr/type>
#include <iostream>

class MyObj
  {
};


int main()
{
  auto tp = rttr::type::get<MyObj*>();

  std::cout << "is arithmetic? " << (int) tp.is_arithmetic() << std::endl;

}
