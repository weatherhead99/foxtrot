#include "pushbullet_api.hh"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace rapidjson;

foxtrot::pushbullet_api::pushbullet_api(const string && api_key)
: lg_("pushbullet_api"), api_key_(api_key)
{
    proto_ = std::make_shared<foxtrot::protocols::CurlRequest>();
}


void foxtrot::pushbullet_api::create_push_channel(const string& title, const string& body,
                                                  const string& channel)
{
    
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    
    writer.StartObject();
    writer.Key("type");
    writer.String("note");
    
    writer.Key("title");
    writer.String(title.c_str());
    
    writer.Key("body");
    writer.String(body.c_str());
    
    writer.Key("channel_tag");
    writer.String(channel.c_str());
    
    
    
}
