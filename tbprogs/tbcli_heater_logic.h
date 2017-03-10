#pragma once
#include "client.h"

int find_archon_heater(const foxtrot::servdescribe& cl);

void enable_disable_heater_output(foxtrot::Client& cl, int devid, bool onoff);
bool is_heater_enabled(foxtrot::Client& cl, int devid);

void set_heater_target(foxtrot::Client& cl, int devid, double target);
double get_heater_target(foxtrot::Client& cl, int devid);
