#pragma once
#include "client.h"

int find_webswitch(const foxtrot::servdescribe& sd);
void do_fill(foxtrot::Client& cl, int devid, double filltime_hours, int relay=1);
