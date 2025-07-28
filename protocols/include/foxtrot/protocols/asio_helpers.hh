#pragma once
#include <optional>
#include <chrono>
#include <memory>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>

using std::optional;
using std::nullopt;
using std::chrono::milliseconds;
namespace asio = boost::asio;

namespace foxtrot{
  namespace protocols {

    namespace detail {
      struct asio_timeout_manager_impl;
    }

    class asio_timeout_manager
    {
    public:
      asio_timeout_manager(asio::any_io_executor exec,
			   optional<milliseconds> timeout=nullopt);

      ~asio_timeout_manager();
      optional<milliseconds>& timeout();

      template<typename T, typename E>
      asio::awaitable<optional<T>, E> wrap_coro_timeout(asio::awaitable<T,E>&& coro_in)
      {
	if(!timeout().has_value())
	  auto coro_passthru = [this, &coro_in] () -> asio::awaitable<optional<T>, E>
	    {
	      co_return ( co_await coro_in) ;
	    };

	else
	  {
	    
	  }
      }

    private:
      std::unique_ptr<detail::asio_timeout_manager_impl> pimpl;
      
    };

    
  }
}
