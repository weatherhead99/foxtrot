#include <iostream>
#include "SerialPort.h"
#include <chrono>

#include <thread>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  
    foxtrot::parameterset params;
    params["baudrate"] = 9600u;
    params["port"] = "/dev/ttyS0";
  
    cout << "serial port init..." << endl;
    
    
    foxtrot::protocols::SerialPort ser(&params);
    
    ser.Init(nullptr);
  
    cout << "serial port write.." << endl;
  
    ser.write("INFO?\n");
    
    cout << "serial port read.. " << endl;
      
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto repl = ser.read(10);
    
    cout << "repl: " << repl << endl;
    
};