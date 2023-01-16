#include <iostream>
#include "APT.h"

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <memory>

using std::cout;
using std::endl;
using std::shared_ptr;

using foxtrot::protocols::SerialPort;
using foxtrot::devices::APT;
using foxtrot::devices::destination;

class Apt_Tester : public APT
{
public:
  Apt_Tester(shared_ptr<SerialPort> proto)
    : APT(proto)
  {
    

  }

  //override this protected method to public for testing purposes
  using APT::transmit_message;
};


//default values for pplxdaq30
foxtrot::parameterset sport_params    
  {
    {"port", "/dev/ttyUSB0"}
  };


int main()
{
  foxtrot::setLogFilterLevel(sl::trace);

  cout << "trying to identify Thorlabs long stage motor controller..." << endl;

  auto sport = std::make_shared<SerialPort>(&sport_params);
  Apt_Tester testdev(sport);

  //send a "genericc"??? hardware info message - try out different destination targets!!
  auto dest = static_cast<destination>(0u);
  
  auto hwinfo = testdev.get_hwinfo(dest);


  

  

}
