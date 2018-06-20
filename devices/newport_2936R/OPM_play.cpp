#include <iostream>
#include <memory>
#include "BulkUSB.h"
#include "SerialPort.h"
#include <chrono>
#include <thread>

#include "newport2936R.h"
#include <backward.hpp>
#include "Logging.h"
#include "DeviceError.h"
#include "ProtocolTimeoutError.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
     backward::SignalHandling sh;
     foxtrot::setDefaultSink();
     foxtrot::setLogFilterLevel(sl::trace);
     
     foxtrot::parameterset params
     { {"port", "/dev/ttyS5"},
       {"read_timeout", 1000u}
       
     };
  
//      auto proto = std::make_shared<foxtrot::protocols::BulkUSB>(&params);
     auto proto = std::make_shared<foxtrot::protocols::BulkUSB>(&params);
     foxtrot::devices::newport2936R OPM(proto);

     try{
      cout << "DS count: " << OPM.getDataStoreCount() << endl;
     }
     catch(foxtrot::DeviceError& err)
     {
       cout << "continuing regardless..." << endl;
     }
     catch(foxtrot::ProtocolTimeoutError& err)
     {
       cout << "continuing regardless of timeout... " << endl;
     };
     
     OPM.setChannel(2);
     OPM.setMode(foxtrot::devices::powermodes::Integrate);
     OPM.setExternalTriggerMode(1);
     OPM.setTriggerEdge(1);
     
     cout << OPM.getAnalogFilter() << endl;
     
     OPM.clearDataStore();
     
     OPM.setDataStoreEnable(true);
     std::this_thread::sleep_for(std::chrono::milliseconds(100));     
     cout << "ops done.." << endl;
     cout << "DS count: " << OPM.getDataStoreCount() << endl;
     
     cout << "DS size: " << OPM.getDataStoreSize() << endl;
     
     auto st = OPM.fetchDataStore(10,21);
     for(auto& v : st)
     {
	cout << "v: " << v << endl;
     }
     
     cout << "trigger state: " << OPM.getTriggerState() << endl;
     
}
