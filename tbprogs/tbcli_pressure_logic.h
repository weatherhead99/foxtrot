#pragma once

#include "client.h"

int find_pressure_gauge(const foxtrot::servdescribe& cl);

double get_pump_pressure(foxtrot::Client& cl, int devid);
double get_cryostat_pressure(foxtrot::Client& cl, int devid);
