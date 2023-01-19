#include <algorithm>
#include <foxtrot/DeviceError.h>
#include "webswitch_plus.h"

using namespace rapidxml;


foxtrot::devices::webswitch_plus::webswitch_plus(std::shared_ptr<CurlRequest>& proto, const std::string& url)
: Device(proto), _url(url), _lg("webswitch_plus")
{
}

foxtrot::devices::webswitch_plus::webswitch_plus(std::shared_ptr<CurlRequest>& proto, const foxtrot::parameterset& params)
: webswitch_plus(proto, std::get<std::string>(params.at("url")))
{
}

const std::string foxtrot::devices::webswitch_plus::getDeviceTypeName() const
{
    return "webswitch_plus";
}


foxtrot::devices::webswitch_plus::~webswitch_plus()
{
}

bool foxtrot::devices::webswitch_plus::GetRelay(int chan)
{
    auto thisproto = std::static_pointer_cast<CurlRequest>(_proto);
    _oss.str("");
    _oss << "http://" << _url << "/state.xml";
    _lg.strm(sl::trace) << "requesting state.xml";
    auto response = thisproto->blocking_get_request(_oss.str());
    
    std::unique_ptr<char> responsec( new char [response.size()+1]);
    std::copy(response.c_str(), response.c_str() + response.size() + 1, responsec.get());
    
    
    _lg.strm(sl::debug) << "parsing XML response";
    _doc.parse<0>(responsec.get());
    
    _lg.strm(sl::trace) << "parsing done";
    
    _oss.str("");
    _oss << "relay" << chan << "state";
    
    _lg.strm(sl::trace) << "nodename: " << _oss.str();
    
    std::string nodename(_oss.str());
    
    auto* datanode = _doc.first_node("datavalues",10);
    
    if(!datanode)
    {
        _lg.strm(sl::error) << "invalid XML node, couldn't parse properly!";
        throw foxtrot::DeviceError("invalid XML node, couldn't parse properly!");
    }
    
    
    auto* relaynode = datanode->first_node(nodename.c_str(),nodename.size());
    
    _lg.strm(sl::trace) << "got node" ;
    
    if(!relaynode)
    {
        _lg.strm(sl::error) << "nullptr for node!";
        throw foxtrot::DeviceError("invalid XML node for relay, perhaps invalid relay number specified");
    }
    
    
    return std::stoi(relaynode->value());
}

void foxtrot::devices::webswitch_plus::SetRelay(int chan, bool onoff)
{
    auto thisproto = std::static_pointer_cast<CurlRequest>(_proto);
    _oss.str("");
    _oss << "http://" << _url << "/state.xml?relay" << chan << "state=" << (int) onoff;
    auto response = thisproto->blocking_get_request(_oss.str());
    
}

RTTR_REGISTRATION {
 using namespace rttr;
 using foxtrot::devices::webswitch_plus;
    
 registration::class_<webswitch_plus>("foxtrot::devices::webswitch_plus")
 .method("GetRelay",&webswitch_plus::GetRelay)
 (
     parameter_names("chan")
 )
 .method("SetRelay",&webswitch_plus::SetRelay)
 (
     parameter_names("chan","onoff")
 );
   
}
