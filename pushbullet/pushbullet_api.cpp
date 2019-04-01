#include "pushbullet_api.hh"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>


#define PUSHBULLET_PUSHES_API "https://api.pushbullet.com/v2/pushes"

using namespace rapidjson;

foxtrot::pushbullet_api::pushbullet_api(const string & api_key)
: lg_("pushbullet_api"), api_key_(api_key)
{
    proto_ = std::make_shared<foxtrot::protocols::CurlRequest>();
}


void foxtrot::pushbullet_api::push_to_channel(const string& title, const string& body,
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
    
    writer.EndObject();
    
    std::vector<string> header{"Access-Token: " + api_key_, "Content-Type: application/json"};
    
    auto repl = proto_->blocking_post_request(PUSHBULLET_PUSHES_API,s.GetString(),&header);
    auto rcode = proto_->get_last_http_response_code();
    
    if(rcode <200  || rcode > 300)
    {
        lg_.strm(sl::debug) << "HTTP response: " << repl;
        //TODO: custom error type here
        
        lg_.strmthrow<std::runtime_error>(sl::error) << "invalid HTTP response code: " << rcode;
        
    }
    
}
