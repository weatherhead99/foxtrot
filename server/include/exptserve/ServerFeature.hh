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
    constexpr static std::string feature_name() const;


    template<typename T>
    void register_handlers(T& impl);

    
    virtual ~ServerFeatureBase();
    
  };



}
