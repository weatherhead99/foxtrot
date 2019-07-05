#include <foxtrot/DeviceError.h>
#include "BSC203.h"


#define DEST_HOST_CONTROLLER 0x01
#define DEST_RACK_CONTROLLER 0x11
#define DEST_BAY_1 0x21
#define DEST_BAY_2 0x22
#define DEST_BAY_3 0x23
#define DEST_GENERIC_USB_HW_UNIT 0x50

#ifdef linux
#include <byteswap.h>
#endif

#include <thread>
#include <chrono>

using std::cout;
using std::endl;

foxtrot::devices::BSC203::BSC203(std::shared_ptr< foxtrot::protocols::SerialPort > proto) : foxtrot::devices::APT(proto)
{
 cout << "BSC203 Calling superclass constructor..." << endl;
}
