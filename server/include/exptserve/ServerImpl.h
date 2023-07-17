#pragma once
#include <memory>
#include <string>
#include <exception>
#include <typeinfo>

#include <grpc++/server_builder.h>
#include <chrono>
#include <vector>
#include <variant>
#include <unordered_map>

#include <foxtrot/Logging.h>

using grpc::ServerBuilder;

using std::shared_ptr;
using std::string;
using std::unique_ptr;




namespace foxtrot{
    using ft_std_variant = std::variant<double,int,bool,std::string>;
    using flagmap =  std::map<std::string, ft_std_variant> ;


  
  class ServerFeatureBase;
  class DeviceHarness;
  struct _ServerImplInternal;
  struct pushbullet_api;
  class SessionManager;

  class ServerImpl{
  public:
    ServerImpl(const string& servcomment, std::shared_ptr<DeviceHarness> harness,
	       const string& connstr, std::chrono::seconds session_length= std::chrono::minutes(10));

    ~ServerImpl();
    
    void setup_notifications(const string& apikey, const string& default_title, const string& default_channel);

    void SetupSSL(const string& crtfile, const string& keyfile,
		  bool force_client_auth = false);


    void add_feature(ServerFeatureBase&& feat);

  protected:
    unique_ptr<pushbullet_api> steal_noti_api();
    const string& servcomment() const;


    void common_build(grpc::ServerBuilder& builder);
    shared_ptr<SessionManager> sesman();
    shared_ptr<DeviceHarness> harness();

  private:

    foxtrot::Logging _lg;
    unique_ptr<_ServerImplInternal> _impl;

  };
  
}
