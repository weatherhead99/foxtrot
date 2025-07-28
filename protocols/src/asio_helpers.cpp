#include <boost/asio/any_io_executor.hpp>
#include <foxtrot/protocols/asio_helpers.hh>

#include <boost/asio/system_timer.hpp>

using namespace foxtrot::protocols;
namespace asio = boost::asio;

struct foxtrot::protocols::detail::asio_timeout_manager_impl
{
  asio::any_io_executor exec;
  optional<milliseconds> timeout;
  asio::system_timer tmr;
  
  asio_timeout_manager_impl(asio::any_io_executor _exec,
			    optional<milliseconds> _timeout)
    : exec(_exec), timeout(_timeout), tmr(_exec) {}
};

asio_timeout_manager::asio_timeout_manager(asio::any_io_executor exec,
                                           optional<milliseconds> timeout)
{
  pimpl = std::make_unique<detail::asio_timeout_manager_impl>(exec, timeout);
}

asio_timeout_manager::~asio_timeout_manager() = default;

optional<milliseconds>& asio_timeout_manager::timeout()
{
  return pimpl->timeout;
}


