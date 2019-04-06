#pragma once
#include <utility>

#include <foxtrot/client/client.h>


int find_archon_heater(const foxtrot::servdescribe& cl);

void enable_disable_heater_output(foxtrot::Client& cl, int devid, bool onoff);
bool is_heater_enabled(foxtrot::Client& cl, int devid);

void set_heater_target(foxtrot::Client& cl, int devid, double target);
double get_heater_target(foxtrot::Client& cl, int devid);

void update_archon_state(const foxtrot::servdescribe& cl, foxtrot::Client& cli);

double get_heater_output(foxtrot::Client& cl, int devid);
std::vector<int> get_heater_coeffs(foxtrot::Client& cl, int devid);


std::pair<double,double> get_temperatures(foxtrot::Client& cl, int devid);




void apply_settings(foxtrot::Client& cl, int devid);




