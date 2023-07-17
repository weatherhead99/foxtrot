#pragma once
#include "ServerFeature.hh"

namespace foxtrot
{
  class pushbullet_api;

  
  class PushbulletBroadcastFeature : public ServerFeatureBase
  {
  public:
    PushbulletBroadcastFeature(const string& api_key,
			       const string& default_channel,
			       const string& default_title);

    void serverbuild_hook(grpc::ServerBuilder& builder) override;
  private:
    unique_ptr<pushbullet_api> _api = nullptr;

  };

}
