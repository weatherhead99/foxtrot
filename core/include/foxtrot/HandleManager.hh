#pragma once
#include <any>
#include <variant>
#include <rttr/type>
#include <unordered_map>
#include <memory>

namespace foxtrot
{
  struct HandleManagerImpl;
  
  class HandleManager
  {

  public:
    static std::shared_ptr<HandleManager> instance();
    rttr::variant& lookup(std::size_t handle);
    rttr::variant pop(std::size_t handle);
    void drop(std::size_t handle);

    std::size_t add(rttr::variant&& var);
    std::size_t add(const rttr::variant& var);

    //singleton enforcement
    HandleManager(HandleManager& other) = delete;
    void operator=(const HandleManager& other) = delete;
    
  protected:
    HandleManager();

    
  private:
    
    
    std::unique_ptr<HandleManagerImpl> _impl;
  };
  

}
