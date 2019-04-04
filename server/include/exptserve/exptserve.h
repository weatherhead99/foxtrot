#pragma once
#include <string>

namespace foxtrot
{
    class DeviceHarness;
}

void dump_setup(const foxtrot::DeviceHarness& harness, const std::string& dumpfile);
