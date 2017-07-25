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
  foxtrot::setLogFilterLevel(sl::info);
  
  
  foxtrot::TelemetryClient client;
  client.ConnectSocket("tcp://pplxdaq13:8000");
  client.Subscribe("testbench");
  
  
  while(true)
  {
    
    auto msg = client.waitMessageSync();
    auto now = boost::posix_time::microsec_clock::universal_time();
    auto deliv_time = (now - msg.timestamp).total_microseconds();
    
    auto dtstr = boost::posix_time::to_iso_string(msg.timestamp);
    cout << "[" << dtstr << " GMT] " << msg.name << " : " << msg.value << endl;
    
  };
  
  
  
  
  
};