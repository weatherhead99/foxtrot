#include "TelemetryClient.h"
#include <iostream>
#include "Logging.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  
  foxtrot::setDefaultSink();
  
  foxtrot::TelemetryClient client;
  client.ConnectSocket("tcp://pplxdaq13:8000");
  client.Subscribe("testbench");
  
  
  while(true)
  {
    cout << "waiting for message...." << endl;
    auto msg = client.waitMessageSync();
    
    cout << "received message!" << endl;
    cout << "---------------------------" << endl;
    
  };
  
  
  
  
  
};