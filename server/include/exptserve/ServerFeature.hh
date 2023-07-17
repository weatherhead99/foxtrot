#pragma once

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
    
    virtual ~ServerFeatureBase();
    
  };


  template<typename T>
  class ServerFeature : public ServerFeatureBase
  {
    using serverimplT = T;
    

  };
  


}
