#include "BroadcastNotificationImpl.h"

foxtrot::BroadcastNotificationLogic::BroadcastNotificationLogic(
    std::unique_ptr<foxtrot::pushbullet_api> api, const string& default_title,
    const string& default_channel)
: lg_("BroadcastNotificationLogic"), api_(std::move(api)), default_title_(default_title),
default_channel_(default_channel)
{
    
}

bool foxtrot::BroadcastNotificationLogic::HandleRequest(
    foxtrot::BroadcastNotificationLogic::reqtp& req,
    foxtrot::BroadcastNotificationLogic::repltp& repl,
    foxtrot::BroadcastNotificationLogic::respondertp& respond, foxtrot::HandlerTag* tag)
{
    lg_.strm(sl::debug) << "got broadcast notification request";
    
    repl.set_msgid(req.msgid());
    
    string title = req.use_default_title() ? default_title_ : req.title() ;
    string channel = req.use_default_channel() ? default_channel_ : req.channel_target();
    
    api_->push_to_channel(title,req.body(),channel);
    respond.Finish(repl,grpc::Status::OK,tag);
    return true;
    
}
