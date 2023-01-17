#include <iostream>
#include "APT.h"

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <string>
#include <memory>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;

using foxtrot::protocols::SerialPort;
using foxtrot::devices::APT;
using foxtrot::devices::destination;
using foxtrot::devices::motor_channel_idents;

class Apt_Tester : public APT
{
public:
  Apt_Tester(shared_ptr<SerialPort> proto)
    : APT(proto)
  {
        stop_update_messages(destination::genericUSB);

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
  auto dest = static_cast<destination>(1u);

  //seems like it sends back src of 0x80. I think I remember this happening before, but can't
  //remember exactly where or why or how...
  auto expd_src = static_cast<destination>(80u);
  
  auto info = testdev.get_hwinfo(dest, expd_src);

  cout << "serial number: " << info.serno << endl;

  std::string modelstr;
  modelstr.resize(8);
  std::copy(info.modelno.begin(), info.modelno.end(), modelstr.begin());
  
  cout << "modelno: " << modelstr << endl;
  cout << "nchans: " << info.nchans << endl;

  std::string notestr;
  notestr.resize(info.notes.size());
  std::copy(info.notes.begin(), info.notes.end(), notestr.begin());
  cout << "notes: " << std::string(notestr) << endl;

  //OK, now let's try getting channel enable state
  auto chan = motor_channel_idents::channel_1;
  auto enablestate = testdev.get_channelenable(dest, chan);

  
  cout << "channelenable: " << (int) enablestate << endl;
  
  auto status = testdev.get_status(destination::genericUSB, chan);
  cout << "position: " << status.position << endl;
  

}
