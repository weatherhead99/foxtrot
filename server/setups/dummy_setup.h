#pragma once

#include <foxtrot/parameter_defs.hh>

using foxtrot::mapofparametersets;


namespace foxtrot {
    class DeviceHarness;
}


extern "C" {
  int setup(foxtrot::DeviceHarness& harness, const mapofparametersets* const paramsets);
}

int setup(std::shared_ptr<foxtrot::DeviceHarness> harness, const mapofparametersets* const paramsets);


