#pragma once

using mapofparametersets = std::map<std::string, std::map<std::string, boost::variant<unsigned,int,std::string>>>;

namespace foxtrot {
    class DeviceHarness;
}   

extern "C" { 
    int setup(foxtrot::DeviceHarness& harness, const mapofparametersets* const paramsets);
}
