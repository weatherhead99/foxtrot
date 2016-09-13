#include <iostream>
#include "SerialPort.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  
    foxtrot::parameterset params;
    params["baudrate"] = 115200u;
    params["port"] = "/dev/ttyS0";
  
    cout << "serial port init..." << endl;
    
    
    foxtrot::protocols::SerialPort ser(&params);
    
    ser.Init(nullptr);
  
    cout << "serial port write..." << endl;
    ser.write("hello");
};