#include "PRT.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char**argv)
{
  
  cout << "resistance value: 95.2 ohms" << endl;
  cout << "calculated temp: " << foxtrot::util::ITL_90_res_to_tmp(95.2, foxtrot::util::PRTsensors::PT100) << endl;
  
}
  
  