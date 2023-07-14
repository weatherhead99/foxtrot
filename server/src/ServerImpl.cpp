#include "ServerImpl.h"

#include <optional>
#include <fstream>

#include "pushbullet_api.hh"
#include <foxtrot/server/SessionManager.hh>
#include <grpc++/security/credentials.h>


using namespace foxtrot;


struct foxtrot::_ServerImplInternal
{
  string connstr;
  shared_ptr<DeviceHarness> harness = nullptr;
  string servcomment;

  //notifications
  unique_ptr<pushbullet_api> noti_api = nullptr;

  shared_ptr<grpc::ServerCredentials> creds = nullptr;
  shared_ptr<foxtrot::SessionManager> sesman = nullptr;

};



foxtrot::ServerImpl::ServerImpl(const string &servcomment,
                                shared_ptr<DeviceHarness> harness,
                                const string &connstr,
				std::chrono::seconds session_length
				)
  : _lg("ServerImpl"){

  _impl = std::make_unique<_ServerImplInternal>();

  _impl->sesman = std::make_shared<foxtrot::SessionManager>(session_length);

  _impl->connstr = connstr;
  _impl->harness = harness;
  _impl->servcomment = servcomment;

  
}



foxtrot::ServerImpl::~ServerImpl()
{

}



void foxtrot::ServerImpl::common_build(grpc::ServerBuilder& builder)
{
				       
  if(!_impl->creds)
    {
      _lg.strm(sl::info) << "no SSL credentials supplied, using insecure ones...";
      _impl->creds = grpc::InsecureServerCredentials();
    }

  builder.AddListeningPort(_impl->connstr, _impl->creds);
  _lg.strm(sl::info) << "server will listen on: " << _impl->connstr;
    

}


void foxtrot::ServerImpl::SetupSSL(const string& crtfile,
				   const string& keyfile,
				   bool force_client_auth)
{
    grpc::SslServerCredentialsOptions::PemKeyCertPair kp;
  
  std::ifstream ifs(crtfile);
  std::stringstream iss;
  
  iss << ifs.rdbuf();
  
  kp.cert_chain = iss.str();
  ifs.close();
  ifs.clear();
  
  
  ifs.open(keyfile);
  iss.str("");
  iss << ifs.rdbuf();
  
  kp.private_key = iss.str();
  
  grpc::SslServerCredentialsOptions opts;
  opts.pem_key_cert_pairs.push_back(kp);
  
  opts.force_client_auth = force_client_auth;

  _impl->creds = grpc::SslServerCredentials(opts);

}



void foxtrot::ServerImpl::setup_notifications(const string& apikey, const string& default_title, const string& default_channel)
{
  _lg.strm(sl::info) << "API key: " << apikey;
  _impl->noti_api = std::make_unique<pushbullet_api>(apikey, default_title, default_channel);

}

unique_ptr<foxtrot::pushbullet_api> foxtrot::ServerImpl::steal_noti_api()
{
  return std::move(_impl->noti_api);
}

const string& foxtrot::ServerImpl::servcomment() const
{
  return _impl->servcomment;
}

shared_ptr<foxtrot::SessionManager> foxtrot::ServerImpl::sesman()
{
  return _impl->sesman;
}

shared_ptr<foxtrot::DeviceHarness> foxtrot::ServerImpl::harness()
{
  return _impl->harness;
}
