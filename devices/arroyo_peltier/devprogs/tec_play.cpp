#include "arroyo5305.hh"
#include <iostream>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>

using std::cout;
using std::endl;
using foxtrot::protocols::SerialPort;

foxtrot::parameterset sport_params
  {
    {"port",  "/dev/ttyTECcontroller"},
    {"baudrate", 38400u}

  };


int main()
{

  foxtrot::setDefaultSink();
  foxtrot::setLogFilterLevel(sl::trace);
  
  cout << "hello" << endl;

  auto sport = std::make_shared<SerialPort>(&sport_params);
  cout << "setting wait..." << endl;



  //auto resp = sport->read_until_endl('\r');

  //cout << "resp: "  << resp << endl;
  
  //cout << "made serial port" << endl;
  
  foxtrot::devices::Arroyo5305 controller(sport);
  cout << "identify.." << endl;
  cout << controller.identify() << endl;
  cout << "version..." << endl;
  cout << controller.version() << endl;


  cout << "current output: " << controller.TECCurrent() << endl;
  cout << "temp_setpoint: " << controller.get_temp_setpoint() << endl;
  cout << "temp actual: " << controller.temp() << endl;


  cout << "setting temp setpoint to 22..." << endl;
  controller.set_temp_setpoint(22.0);

  cout << "reading back setpoint..." << endl;
  cout << "temp_setpoint: " << controller.get_temp_setpoint() << endl;


  cout << "total uptime... " << controller.total_centiseconds_uptime() << endl;
  //cout << "made controller" << endl;

  cout << "timer.... " << controller.total_centiseconds_timer() << endl;
  //auto vers = controller.identify();

  cout << "voltage: " << controller.voltage() << endl;
  //cout << "firmware version: " << vers << endl;

  cout << "P,I,D: " ;

  auto PID = controller.get_PID();
  for(auto& v : PID)
    cout << v << ",";
  cout << endl;

  //DISABLED only available in firmware 2.0
  // auto inv = controller.get_ITEInvert();
  // cout << "is ITE inverted?"  << (int)inv << endl;
  // cout << "flipping... ";
  // controller.set_ITEInvert(!inv);
  // cout << "is ITE inverted? " << (int)controller.get_ITEInvert() << endl;

  controller.set_heatcool(foxtrot::devices::TECHeatCool::COOL);
  auto hc = controller.get_heatcool();
  cout << "heat/cool: " << static_cast<short unsigned>(hc) << endl;

  controller.set_heatcool(foxtrot::devices::TECHeatCool::BOTH);
   hc = controller.get_heatcool();
   cout << "heat/cool: " << static_cast<short unsigned>(hc) << endl;
  

   auto [speed, mode, delay] = controller.get_fanspeed();

   cout << "speed: " << speed << "mode: " << mode << ", delay: "<< delay << endl;
   

   auto stypeblah = controller.get_sensortype(1);
   cout << "stypeblah: " << (int)stypeblah << endl;

   
   auto stype2 = controller.get_sensortype(12);
   cout << "stype2: " << (int)stype2 << endl;

      
   auto errs = controller.get_errors();
   for(auto& err: errs)
     cout << "ERROR:" << err << endl;;
   

   controller.set_sensortype(12, foxtrot::devices::TECSensorType::THERMISTOR_10uA);
   errs = controller.get_errors();
   for(auto& err: errs)
     cout << "ERROR:" << err << endl;;

   stype2 = controller.get_sensortype(12);
   cout << "stype2: " << (int)stype2 << endl;

   
}
