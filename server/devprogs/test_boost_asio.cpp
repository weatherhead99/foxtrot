#include <iostream>

#include <boost/asio/use_awaitable.hpp>

using std::cout;
using std::endl;


int main()
{

#ifdef BOOST_ASIO_HAS_CO_AWAIT
  cout << "has coro " << endl;
#else
  cout << "doesn't have coro"  << endl;
#endif

#ifdef BOOST_ASIO_HAS_STD_COROUTINE
  cout << "has std coro" << endl;
#else
  cout << "no std coro" << endl;

#endif

}
