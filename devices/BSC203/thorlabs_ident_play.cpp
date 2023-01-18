#include <iostream>
#include "APT.h"

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <string>
#include <memory>
#include <iomanip>

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
    _serport->flush();
        stop_update_messages(destination::genericUSB);
	stop_motor_messages(destination::genericUSB);
  }

  //override this protected method to public for testing purposes
  using APT::transmit_message;
  using APT::stop_update_messages;
  using APT::start_update_messages;
};


//default values for pplxdaq30
foxtrot::parameterset sport_params    
  {
    {"port", "/dev/ttyUSB0"}
  };


int main()
{
  foxtrot::setLogFilterLevel(sl::info);

  cout << "trying to identify Thorlabs long stage motor controller..." << endl;

  auto sport = std::make_shared<SerialPort>(&sport_params);
  Apt_Tester testdev(sport);


  //seems like it sends back src of 0x80. I think I remember this happening before, but can't
  //remember exactly where or why or how...
  
  auto info = testdev.get_hwinfo(destination::genericUSB);

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
  auto enablestate = testdev.get_channelenable(destination::genericUSB, chan);

  cout << "channelenable: " << (int) enablestate << endl;


  //let's try to enable the channel...
  testdev.set_channelenable(destination::genericUSB, chan, true);

  enablestate = testdev.get_channelenable(destination::genericUSB, chan);
  cout << "channelenable: " << (int) enablestate << endl;

  
  //testdev.start_update_messages(destination::genericUSB);
  auto status = testdev.get_status(destination::genericUSB, chan);
  //testdev.stop_update_messages(destination::genericUSB);
  cout << "position: " << status.position << endl;
  cout << "encoder count: " << status.enccount << endl;
  cout << "status bits: " << std::bitset<32>(status.statusbits)  << endl;

  auto velparams = testdev.get_velocity_params(destination::genericUSB, chan);

  cout << "min velocity: " << velparams.minvel << endl;
  cout << "max velocity: " << velparams.maxvel << endl;
  cout << "acceleration: " << velparams.acceleration << endl;
  
  auto tgt = status.position + 2000;

  auto tmneed = testdev.estimate_abs_move_time(destination::genericUSB, chan,
					       tgt);


  cout << "estimated move time (ms): " << tmneed.count() << endl;

  cout << "going to try a move! " << endl;
  testdev.absolute_move_blocking(destination::genericUSB, chan, tgt);

  cout << "move complete" << endl;

  status = testdev.get_status(destination::genericUSB, chan);

  cout << "position: " << status.position << endl;
  
}
