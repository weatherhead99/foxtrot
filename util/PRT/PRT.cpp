#include "PRT.h"
#include <iostream>
using std::endl;
using std::cout;

#include <cmath>
#include <complex>
#include <gsl/gsl_poly.h>
#include <memory>
#include <array>
#include <algorithm>

double foxtrot::util::ITL_90_temp_to_res(double temp_degC, PRTsensors sensor )
{
  
  double R_0 = static_cast<double>(sensor);
  double C = temp_degC < 0. ? 0. : ITL90_C;
  
  double R_t = R_0 * (1 + ITL90_A*temp_degC + ITL90_B*temp_degC*temp_degC + C * (temp_degC - 100) * temp_degC*temp_degC*temp_degC );
  
  return R_t;
}



double foxtrot::util::ITL_90_res_to_tmp(double res_ohms, foxtrot::util::PRTsensors sensor)
{
  double R_0 = static_cast<double>(sensor);
  
  //try calculating using quadratic
  double root1 = 0.;
  double root2 = 0.;
  
  
  auto poly_deleter = [] (gsl_poly_complex_workspace* wspace){gsl_poly_complex_workspace_free(wspace);};
  std::unique_ptr<gsl_poly_complex_workspace,decltype(poly_deleter)> wspace(gsl_poly_complex_workspace_alloc(5),poly_deleter);
  
  std::array<double,8> result;
  
  std::array<double,5> coeffs = { 1. - res_ohms/R_0 , ITL90_A, ITL90_B, -100. * ITL90_C, ITL90_C };
  
  auto status = gsl_poly_complex_solve(coeffs.data(),coeffs.size(),wspace.get(),result.data());
 
  std::array<std::complex<double>,4> result_complex;
  auto itout = result_complex.begin();
  
  //copy results into complex numbers
  for(auto it = result.begin(); it != result.end();)
  {
    double real = *it++;
    double imag = *it++;
    *itout = std::complex<double>{real,imag};
    itout++;
  }

  auto isvalid = [] (const std::complex<double>& t){ 
    if( (t.imag() == 0. )  &&  (t.real() > -200.) && t.real() < 850. )
    {
      return true;
    }
    return false;
  };

  
  
  //only want real numbers
  
  auto findit = std::find_if(result_complex.begin(), result_complex.end(), isvalid);
  
  return (*findit).real();   
}

