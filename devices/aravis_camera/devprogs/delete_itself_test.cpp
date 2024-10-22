#include <memory>
#include <iostream>

using std::cout;
using std::endl;
using std::unique_ptr;


struct My
{
  void suicide()
  {
    delete this;
  }


};


int main()
{

  auto p = std::make_unique<My>();
  
  p->suicide();
  cout << "p has now committed suicide" << endl;


}
