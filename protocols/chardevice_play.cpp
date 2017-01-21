#include "characterdevice.h"
#include <iostream>
#include "PRT.h"
#include <string>


using std::cout; 
using std::endl;

int main(int argc, char** argv)
{
  foxtrot::parameterset params;
  params["devnode"] = "/dev/usbtmc0";
  
  
  cout << "char device init..." <<endl;
  
  foxtrot::protocols::characterdevice dev(&params);
  dev.Init(nullptr);
  
  cout << "char device write..." << endl;
  dev.write(":MEAS:FRES?");
  
  cout << "char device read..." << endl;
  auto resistance = std::stod(dev.read_until_endl());
  cout << "resistance: " << resistance << "ohms" << endl;
  
  //use ITL-90 to find temperature
  
  auto temp_C = foxtrot::util::ITL_90_res_to_tmp(resistance,foxtrot::util::PRTsensors::PT100); 
  
  cout << "temperature: " << temp_C << " degC" << endl;
  
  
}