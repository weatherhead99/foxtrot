#include "wl_sweep_io.h"
#include <vector>
#include <string>
#include <algorithm>

#include <gsl/gsl_statistics.h>
#include <cmath>

#include <iostream>

using std::cout;
using std::endl;

wl_sweep_table::wl_sweep_table(const string& fname, int readings_per_wl)
: _fits(fname, CCfits::Write)
{
  
  
  
  //column names
  const std::vector<std::string> colnames{ "wavelength", "current", "mean", "SD", "responsivity"};
  //column formats
  auto rpwstr  = std::to_string(readings_per_wl);
  const std::vector<std::string> colfmts{ "1D", rpwstr+"D", "1D", "1D", "1D" };
  
  //column units
  const std::vector<std::string> colunits {"nm", "A", "A", "A", "A/W"};
  
  _table = _fits.addTable("data",0,colnames,colfmts,colunits);
  
  
  
}

void wl_sweep_table::add_data_row(const std::vector< double > data, const double responsivity, const double wl)
{
  auto mn = gsl_stats_mean(data.data(), 1,data.size());
  auto sd = gsl_stats_sd(data.data(),1,data.size());
  
  auto numrows = _table->rows(); 
  
  cout << "numrows: " << numrows << endl;
  cout << "reading: " << mn << " +- " << sd << endl;
  

  _table->insertRows(numrows);
  
  _table->column(1).write(std::vector<double>{wl},numrows+1);
  _table->column(2).write(data,1,numrows+1);
  _table->column(3).write(std::vector<double>{mn},numrows+1);
  _table->column(4).write(std::vector<double>{sd},numrows+1);
  _table->column(5).write(std::vector<double>{responsivity},numrows+1);
//   
}



wl_sweep_table::~wl_sweep_table()
{

}

