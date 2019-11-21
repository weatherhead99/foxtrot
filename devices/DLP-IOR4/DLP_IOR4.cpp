#include "DLP_IOR4.hh"

const foxtrot::parameterset IOR4_serial_params
{
    {"baudrate", 9600u},
    {"stopbits", 1u},
    {"bits", 8u},
    {"parity", "none"},
    {"flowcontrol", "none"}
};

using namespace foxtrot::devices;

DLP_IOR4::DLP_IOR4(shared_ptr<foxtrot::protocols::SerialPort> proto, 
                   const std::array<DLP_init_relay_state, 4>& states)
: _relaystatus(states), _lg("DLP_IOR4")
{
    
}

