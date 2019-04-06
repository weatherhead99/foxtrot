#pragma once

#include "client.h"

int find_monochromator(const foxtrot::servdescribe& cl);

void set_wavelength_dumb(foxtrot::Client& cl, int devid, double wl);

void set_filter_dumb(foxtrot::Client& cl, int devid, int filter);
void set_shutter_dumb(foxtrot::Client& cl, int devid, bool onoff);
void set_grating_dumb(foxtrot::Client& cl, int devid, int grating);

void set_wavelength_smart(foxtrot::Client& cl, int devid, double wl);


double get_wavelength(foxtrot::Client& cl, int devid);
int get_filter(foxtrot::Client& cl, int devid);
bool get_shutter(foxtrot::Client& cl, int devid);
int get_grating(foxtrot::Client& cl, int devid);