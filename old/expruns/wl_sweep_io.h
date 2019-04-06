#pragma once
#include <string>
#include <CCfits/CCfits>
#include <vector>

class wl_sweep_table
{
public:
  wl_sweep_table(const std::string& fname, int readings_per_wl);
  
  void add_data_row(const std::vector<double> data, const double responsivity, const double wl);
  
  ~wl_sweep_table();
  
  CCfits::FITS _fits;
  CCfits::Table* _table;
  
  
};