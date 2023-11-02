#pragma once
#include <string>

namespace grpc
{
  class ServerBuilder;
}


namespace foxtrot
{
 
  class ServerFeatureBase
  {
  public:
    virtual void serverbuild_hook(grpc::ServerBuilder& builder) = 0;
    virtual void postbuild_hook() = 0;
    constexpr static std::string feature_name();


    template<typename T>
    void register_handlers(T& impl);

    
    virtual ~ServerFeatureBase();

  protected:
    static std::size_t next_id() noexcept;
    
  };

  template<typename T>
  class ServerFeature : public ServerFeatureBase
  {
    static std::size_t feat_type_id() noexcept
    {
      static std::size_t typeid_ = ServerFeatureBase::next_id();
      return typeid_;
    };

  };



}
