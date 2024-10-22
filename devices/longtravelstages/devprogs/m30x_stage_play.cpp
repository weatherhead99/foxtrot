#include "M30X.hh"
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <iostream>
#include <bitset>

using foxtrot::protocols::SerialPort;

foxtrot::parameterset sport_params{{"port", "/dev/ttyM30Xstage"},
                                   {"baudrate", 115200u}};

using foxtrot::devices::destination;
using foxtrot::devices::motor_channel_idents;
using foxtrot::devices::dcstatus;
using std::cout;
using std::endl;

int main() {
  foxtrot::setDefaultSink();
  foxtrot::setLogFilterLevel(sl::debug);
  //foxtrot::setLogFilterLevel(sl::trace);

  cout << "making serial port..." << endl;
  auto sport = std::make_shared < SerialPort > ( & sport_params);

  cout << "making motor controller object" << endl;
  foxtrot::devices::M30X controller(sport);

  cout << "getting hwinfo" << endl;
  auto hwinfo = controller.get_hwinfo();
  cout << "serial number: " << hwinfo.serno << endl;
  cout << "type: " << hwinfo.type << endl;
  std::string model(hwinfo.modelno.begin(), hwinfo.modelno.end());
  cout << "model number: " << model << endl;

  cout << "channel 1 enabled: " << (int) controller.get_channelenable(motor_channel_idents::channel_1) << endl;
  controller.set_channelenable(motor_channel_idents::channel_1, true);
  cout << "channel 1 enabled: " << (int) controller.get_channelenable(motor_channel_idents::channel_1) << endl;

  auto stat = controller.get_status(motor_channel_idents::channel_1);
  cout << "position: " << std::get < dcstatus > (stat).position << endl;
  std::bitset < 32 > sbits = std::get < dcstatus > (stat).statusbits;
  cout << "status bits: " << sbits << endl;
  cout << "status bits (hex): " << std::get < dcstatus > (stat).statusbits << endl;

  //    cout << "getting limit switch params..." << endl;
  //    auto limswitchparams = controller.get_limitswitchparams(destination::genericUSB, motor_channel_idents::channel_1);
  //
  //    cout << "channel:" << limswitchparams.chan_ident << endl;
  //    cout << "CWsoft:" << limswitchparams.CWsoft << endl;
  //    cout << "CCWsoft: " << limswitchparams.CCWsoft << endl;
  //    cout << "limitMode:" << limswitchparams.limitMode << endl;
  //
  //    limswitchparams.limitMode = 0x02;
  // long mm = 10000;
  //    limswitchparams.CWsoft = 20*mm;
  //    limswitchparams.CCWsoft = -20*mm;
  //
  //
  //    controller.set_limitswitchparams(destination::genericUSB, limswitchparams);

  controller.set_soft_limits(200000, -200000);

  auto limswitchparams = controller.get_limitswitchparams(motor_channel_idents::channel_1);

  cout << "new limitMode: " << limswitchparams.limitMode << endl;
  cout << "CWsoft:" << limswitchparams.CWsoft << endl;
  cout << "CCWsoft: " << limswitchparams.CCWsoft << endl;

  stat = controller.get_status(motor_channel_idents::channel_1);
  cout << "position: " << std::get < dcstatus > (stat).position << endl;
  sbits = std::get < dcstatus > (stat).statusbits;
  cout << "status bits: " << sbits << endl;

  cout << "doing absolute move" << endl;

  //NOTE: M30X moves need bay 1 destination!
  controller.absolute_move_blocking(motor_channel_idents::channel_1, 0);

  cout << "testing position consistancy" << endl;
  for (int i = 0; i < 0; i++) {
    controller.absolute_move_blocking(motor_channel_idents::channel_1, 100000);
    controller.absolute_move_blocking( motor_channel_idents::channel_1, 0);
  }

  cout << "trying safe home move" << endl;
  controller.safe_home_move(true);
}
