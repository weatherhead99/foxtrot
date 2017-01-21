#pragma once
#include <vector>


namespace foxtrot{

namespace util { 
    
    const double ITL90_A = 3.9083E-3;
    const double ITL90_B = -5.7750E-7;
    const double ITL90_C = -4.1830E-12;
    
    enum class PRTsensors : short int
    {
    PT100 = 100,
    PT1000 = 1000
      
    };


    double ITL_90_temp_to_res(double temp_degC, PRTsensors sensor);

    
    double ITL_90_res_to_tmp(double res_ohms, PRTsensors sensor);
    
			      
}

}