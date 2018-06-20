#include <iostream>
#include <memory>
#include "BulkUSB.h"
#include "SerialPort.h"
#include <chrono>
#include <thread>

#include "newport2936R.h"
#include <backward.hpp>
#include "Logging.h"

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
     
     auto dsenable = OPM.getDataStoreEnable();
     cout << "dsenable: " << dsenable << endl;

     //enable data store
     cout << "DS size: " << OPM.getDataStoreSize() << endl;
     cout << "DS count: " << OPM.getDataStoreCount() << endl;

     OPM.setBufferBehaviour(0);
     OPM.setDataStoreEnable(true);
     dsenable = OPM.getDataStoreEnable();
     cout << "dsenable: " << dsenable << endl;


     std::this_thread::sleep_for(std::chrono::milliseconds(100));
     cout << "DS count: " << OPM.getDataStoreCount() << endl;

     auto ret = OPM.cmd("PM:DS:GET? 1-20");
     cout << "return: " << ret << endl;

     OPM.clearDataStore();
     cout << "DS count: " << OPM.getDataStoreCount() << endl;


     

}
