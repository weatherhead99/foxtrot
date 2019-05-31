#include "idscamera.h"

#include <ueye.h>

#include <foxtrot/DeviceError.h>

using namespace foxtrot;
using namespace foxtrot::devices;



namespace detail {
    void check_ueye_error(int32_t ret, foxtrot::Logging* lg)
    {
        if(ret != IS_SUCCESS)
        {
            std::string msg {"ueye API Error: " + std::to_string(ret)};
            if(lg)
                lg->strm(sl::error) << msg;
            throw DeviceError(msg);
        }
    
    };
}


using foxtrot::devices::detail::check_ueye_error;

idscamera::idscamera(const uint32_t*  camid)
: Device(nullptr), _lg("idscamera")
{
    if(camid)
        _camhandle = *camid;
    check_ueye_error(is_InitCamera(&_camhandle, nullptr));
    
};

idscamera::~idscamera()
{
    try
    {
        check_ueye_error(is_ExitCamera(_camhandle));
    }
    catch(DeviceError& err)
    {
        //do nothing, have already logged the error and this is a destructor
    }
}

uint32_t idscamera::getPixelClock()
{
    return read_ueye_parameter_command<uint32_t>(is_PixelClock, IS_PIXELCLOCK_CMD_GET);
};

void idscamera::setPixelClock(uint32_t pclock)
{
    set_ueye_parameter_command(is_PixelClock, IS_PIXELCLOCK_CMD_SET, pclock);
}

double idscamera::getExposure()
{
    return read_ueye_parameter_command<double>(is_Exposure, IS_EXPOSURE_CMD_GET_EXPOSURE);
}

void idscamera::setExposure(double exp)
{
    set_ueye_parameter_command(is_Exposure, IS_EXPOSURE_CMD_SET_EXPOSURE, exp);
}


RTTR_REGISTRATION{
    rttr::registration::class_<idscamera>("foxtrot::devices::idscamera")
    .constructor<const uint32_t* const>()
    .property("PixelClock", &idscamera::getPixelClock, &idscamera::setPixelClock)
    .property("Exposure", &idscamera::getExposure, &idscamera::setExposure)
    ;
    
    
}
