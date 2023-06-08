#include <foxtrot/HandleManager.hh>
#include <random>
#include <chrono>

using namespace foxtrot;

static std::shared_ptr<foxtrot::HandleManager> _global_instance = nullptr;


struct foxtrot::HandleManagerImpl {
  std::unordered_map<std::size_t, rttr::variant> values;
  std::random_device rd;
  std::mt19937_64 twister = std::mt19937_64(rd());
  
};

foxtrot::HandleManager::HandleManager() {

  _impl = std::make_unique<foxtrot::HandleManagerImpl>();

  auto now = std::chrono::system_clock::now();
  _impl->twister.seed(std::chrono::system_clock::to_time_t(now));
  
}    


std::shared_ptr<HandleManager> foxtrot::HandleManager::instance()
{
  if(! _global_instance)
    {
      _global_instance.reset(new foxtrot::HandleManager);
    }

  return _global_instance;
}


rttr::variant& HandleManager::lookup(std::size_t handle)
{
  return  _impl->values.at(handle);

}


rttr::variant HandleManager::pop(std::size_t handle)
{
  //NOTE: maybe use .extract() instead of this??
  
  rttr::variant out;
  auto it = _impl->values.find(handle);
  if(it == _impl->values.end())
    throw std::out_of_range("no such handle found!");
  out = *it;

  _impl->values.erase(it);

  return out;
}

void HandleManager::drop(std::size_t handle) { _impl->values.erase(handle); }

std::size_t HandleManager::add(rttr::variant&& var)
{
  std::size_t newkey = _impl->twister();
  _impl->values.insert_or_assign(newkey, std::move(var));
  
  return newkey;
}

std::size_t HandleManager::add(const rttr::variant& var)
{
  std::size_t newkey = _impl->twister();
  _impl->values.insert_or_assign(newkey, var);
  return newkey;

}




