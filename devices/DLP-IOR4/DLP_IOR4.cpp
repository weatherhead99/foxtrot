#include "DLP_IOR4.hh"
#include <thread>
#include <chrono>
#include <foxtrot/DeviceError.h>

#include <rttr/registration>

//see https://docs.rs-online.com/9004/0900766b814bfd24.pdf for details

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
                   const std::string& comment,
                   const std::array<DLP_init_relay_state, 4>& states)
: _relaystatus(states), _lg("DLP_IOR4"), Device(proto, comment), _serport(proto)
{
    _lg.strm(sl::info) << "setting up DLP_IO4 relay device...";
    _serport->Init(&IOR4_serial_params);
    _serport->flush();
    
    _lg.strm(sl::info) << "DLP_IO4 initialization done";
}

const std::string foxtrot::devices::DLP_IOR4::getDeviceTypeName() const
{
    return "DLP_IOR4";
}

void DLP_IOR4::checkAlive()
{
    _serport->write("'"); 
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    unsigned actlen = 0;
    auto repl = _serport->read(1, &actlen);
    if( actlen != 1)
        throw foxtrot::DeviceError("invalid number of bytes read");
    
    if(repl != "R")
    {
        _lg.strm(sl::error) << "return code was: " << repl;
        throw foxtrot::DeviceError("keepalive ping failed");
    }
        
}

bool foxtrot::devices::DLP_IOR4::getRelay(int n) const
{
    if( _relaystatus.at(n) == DLP_init_relay_state::do_not_change)
        throw foxtrot::DeviceError("relay is in an unknown state!");

    return (_relaystatus.at(n) == DLP_init_relay_state::on) ? true: false;
}

void foxtrot::devices::DLP_IOR4::setRelay(int n, bool onoff)
{
    char setchar;
    switch(n)
    {
        case(0) : setchar = onoff ? '1' : 'Q';  break;
        case(1) : setchar = onoff ? '2' : 'W'; break;
        case(2) : setchar = onoff ? '3' : 'E'; break;
        case(3) : setchar = onoff ? '4' : 'R'; break;
        default:
            throw std::out_of_range("invalid relay number specified");
            
    }
    
    _serport->write(std::string{setchar});
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
}


RTTR_REGISTRATION
{
    using namespace rttr;
    using foxtrot::devices::DLP_IOR4;
    registration::class_<DLP_IOR4>("foxtrot::devices::DLP_IOR4")
    .method("checkAlive", &DLP_IOR4::checkAlive)
    .method("getRelay", &DLP_IOR4::getRelay) (parameter_names("n"))
    .method("setRelay", &DLP_IOR4::setRelay) (parameter_names("n", "onoff"));
    
}

