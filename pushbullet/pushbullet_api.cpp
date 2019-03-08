#include "pushbullet_api.hh"

foxtrot::pushbullet_api::pushbullet_api(const string && api_key)
: lg_("pushbullet_api"), api_key_(api_key)
{
    proto_ = std::make_shared<foxtrot::protocols::CurlRequest>();
}

