#include <foxtrot/ServerError.h>

#include "BroadcastNotificationImpl.h"
#include "pushbullet_api.hh"


foxtrot::BroadcastNotificationLogic::BroadcastNotificationLogic(
    std::unique_ptr<foxtrot::pushbullet_api> api)
: lg_("BroadcastNotificationLogic"), api_(std::move(api))
{
    
}

bool foxtrot::BroadcastNotificationLogic::HandleRequest(
    foxtrot::BroadcastNotificationLogic::reqtp& req,
    foxtrot::BroadcastNotificationLogic::repltp& repl,
    foxtrot::BroadcastNotificationLogic::respondertp& respond, foxtrot::HandlerTag* tag)
{
    lg_.strm(sl::debug) << "got broadcast notification request";
    
    repl.set_msgid(req.msgid());

    optstring title, channel;

    if(!req.use_default_title())
      {
	string temptitle = req.title();
	title = std::ref(temptitle);
      }
    if(!req.use_default_channel())
      {
	string tempchan = req.channel_target();
	channel = std::ref(tempchan);
      }

    
    if(!api_)
    {
        throw foxtrot::ServerError("broadcast notifications not enabled on this server");
    }
    
    api_->push_to_channel_with_defaults(req.body(), title, channel);
    
    respond.Finish(repl,grpc::Status::OK,tag);
    return true;
    
}
