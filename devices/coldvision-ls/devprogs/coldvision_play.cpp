#include <iostream>
#include "coldvision.hh"

foxtrot::parameterset sport_params
  {
    {"port",  "/dev/ttyACM2"},
    {"baudrate", 115200u}

  };

using std::cout;
using std::endl;


int main()
{

  foxtrot::setDefaultSink();
  foxtrot::setLogFilterLevel(sl::trace);

  cout << "hello";

  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);

  foxtrot::devices::ColdVisionLS cvision(sport);


  cout << "board temp: " <<  cvision.get_boardTemp() << endl;
  cout << "input voltage: " << cvision.get_inputVoltage() << endl;


  cout << "equalizer status: " << (int) static_cast<unsigned char>(cvision.get_equalizerStatus()) << endl;

  cout << "raw light feedback: " << cvision.get_lightFeedbackRaw() << endl;

  cout << "serial number: "<< cvision.get_SerialNumber() << endl;
  cout << "model number: "<< cvision.get_ModelNumber() << endl;

  //demo mode offset
  cvision.set_demoMode(false);
  
}
