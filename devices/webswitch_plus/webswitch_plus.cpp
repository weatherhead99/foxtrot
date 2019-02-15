#include "webswitch_plus.h"
#include <algorithm>

using namespace rapidxml;


foxtrot::devices::webswitch_plus::webswitch_plus(std::shared_ptr<CurlRequest>& proto, const std::string& url)
: Device(proto), _url(url), _lg("webswitch_plus")
{
}

foxtrot::devices::webswitch_plus::webswitch_plus(std::shared_ptr<CurlRequest>& proto, const foxtrot::parameterset& params)
: webswitch_plus(proto, boost::get<std::string>(params.at("url")))
{
}

const std::string foxtrot::devices::webswitch_plus::getDeviceTypeName() const
{
    return "webswitch_plus";
}


foxtrot::devices::webswitch_plus::~webswitch_plus()
{
}

bool foxtrot::devices::webswitch_plus::GetRelay(unsigned short chan)
{
    auto* thisproto = std::static_pointer_cast<CurlRequest>(_proto);
    _oss.str("");
    _oss << "http://" << _url << "/state.xml";
    _lg.strm(sl::trace) << "requesting state.xml";
    auto response = thisproto->blocking_get_request(_oss.str());
    
    std::unique_ptr<char> responsec( new char [response.size()+1]);
    
    _lg.strm(sl::debug) << "parsing XML response";
    _doc.parse<0>(responsec.get());
    
    _oss.str("");
    _oss << "relay" << chan << "state";
    std::string nodename(_oss.str());
    
    auto* relaynode = _doc.first_node(nodename.c_str(),nodename.size());
    return std::stoi(relaynode->value());
}

void foxtrot::devices::webswitch_plus::SetRelay(unsigned short chan, bool onoff)
{
    auto* thisproto = std::static_pointer_cast<CurlRequest>(_proto);
    _oss.str("");
    _oss << "http://" << _url << "/state.xml?relay" << chan << "state=" << (int) onoff;
    auto response = thisproto->blocking_get_request(_oss.str());
    
}


