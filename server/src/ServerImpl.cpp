#include "ServerImpl.h"


#include "pushbullet_api.hh"


using namespace foxtrot;


struct foxtrot::_ServerImplInternal
{
  string connstr;
  shared_ptr<DeviceHarness> harness;
  string servcomment;

  //notifications
  unique_ptr<pushbullet_api> noti_api;
  bool notifications_enabled;
  string default_channel;
  string default_title;

};




foxtrot::ServerImpl::ServerImpl(const string &servcomment,
                                shared_ptr<DeviceHarness> harness,
                                const string &connstr)
  : _lg("ServerImpl"){

  _impl = std::make_unique<_ServerImplInternal>();
  

  _impl->connstr = connstr;
  _impl->harness = harness;
  _impl->servcomment = servcomment;

  
}



foxtrot::ServerImpl::~ServerImpl()
{

}


void foxtrot::ServerImpl::setup_notifications(const string& apikey, const string& default_title, const string& default_channel)
{
  _lg.strm(sl::info) << "API key: " << apikey;
  _impl->noti_api = std::make_unique<pushbullet_api>(apikey);
  _impl->notifications_enabled = true;
  _impl->default_channel = default_channel;
  _impl->default_title = default_title;

}

unique_ptr<foxtrot::pushbullet_api> foxtrot::ServerImpl::steal_noti_api()
{
  return std::move(_impl->noti_api);
}
