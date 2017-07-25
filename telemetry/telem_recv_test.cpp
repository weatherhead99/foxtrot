#include "TelemetryClient.h"
#include <iostream>
#include "Logging.h"
#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>



using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  
  foxtrot::setDefaultSink();
//   foxtrot::setLogFilterLevel(sl::info);
  
  
  foxtrot::TelemetryClient client;
  client.ConnectSocket("tcp://pplxdaq13:8000");
  client.Subscribe("testbench");
  
  
  while(true)
  {
    cout << "waiting for message...." << endl;
    auto msg = client.waitMessageSync();
    
    cout << "received message!" << endl;
    cout << "---------------------------" << endl;
    auto dtstr = boost::posix_time::to_iso_string(msg.timestamp);
    cout << "message timestamp: " << dtstr << " GMT" << endl;
    cout << "message name: " << msg.name << endl;
    cout << "message value: " << msg.value << endl;
    
    
    
  };
  
  
  
  
  
};