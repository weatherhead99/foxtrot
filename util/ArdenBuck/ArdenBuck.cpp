#include "ArdenBuck.h"
#include <cmath>

double foxtrot::util::arden_buck_vapour_pressure(double temp)
{
    
    if(temp < 0)
    {
        return 0.61115 * std::exp( (23.036 - (temp / 333.7) ) * (temp / (279.82 + temp) ) );
    }
    else
    {
        
         return 0.61121 * std::exp( ( 18.678 - (temp / 234.5) ) * (temp / (257.14 + temp)) );
        
    }
    
    
}


