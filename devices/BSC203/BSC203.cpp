#include "BSC203.h"
#include <DeviceError.h>


#define DEST_HOST_CONTROLLER 0x01
#define DEST_RACK_CONTROLLER 0x11
#define DEST_BAY_1 0x21
#define DEST_BAY_2 0x22
#define DEST_BAY_3 0x23
#define DEST_GENERIC_USB_HW_UNIT 0x50

#define MGMSG_MOD_IDENTIFY 0x0223

#include <byteswap.h>



const foxtrot::parameterset bsc203_class_params
{
  {"baudrate", 115200u},
  {"stopbits", 1u},
  {"flowcontrol", "hardware"},
  {"parity", "none"}
  
  
};


foxtrot::devices::BSC203::BSC203(std::shared_ptr< foxtrot::protocols::SerialPort > proto) 
: foxtrot::Device(proto), _serport(proto)
{
  
  _serport->Init(&bsc203_class_params);

}


void foxtrot::devices::BSC203::identify_module(foxtrot::devices::destination dest)
{
  transmit_message(MGMSG_MOD_IDENTIFY,0,0,dest);
}


void foxtrot::devices::BSC203::transmit_message(short unsigned int opcode, unsigned char p1, unsigned char p2, 
						destination dest, destination src)
{
  
  unsigned char* optpr = reinterpret_cast<unsigned char*>(&opcode);
  
  std::array<unsigned char, 6> header{ optpr[1], optpr[0], p1, p2, static_cast<unsigned char>(dest) ,static_cast<unsigned char>(src)};
  
  _serport->write(std::string(header.begin(), header.end()));

}






