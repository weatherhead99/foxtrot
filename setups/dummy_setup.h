#pragma once
#include "dummy_setup_export.h"

using mapofparametersets = std::map<std::string, std::map<std::string, boost::variant<unsigned,int,std::string>>>;

namespace foxtrot {
    class DeviceHarness;
}   

extern "C" { 
    DUMMY_SETUP_EXPORT int setup(foxtrot::DeviceHarness& harness, const mapofparametersets* const paramsets);
}
