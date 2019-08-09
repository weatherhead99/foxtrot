#include "idscamera.h"
#include <fstream>
#include <ueye.h>
#include <iostream>
#include <foxtrot/DeviceError.h>
#include <ctime>
#include <thread>
#include <chrono>

using namespace foxtrot;
using namespace foxtrot::devices;

using std::cout;
using std::endl;

namespace foxtrot {
  namespace devices {
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
}
}


using foxtrot::devices::detail::check_ueye_error;

idscamera::idscamera(const uint32_t*  camid)
: Device(nullptr), _lg("idscamera")
{
    if(camid)
    {
        _camhandle = *camid;
    }
    check_ueye_error(is_InitCamera(&_camhandle, nullptr));
    
    check_ueye_error(is_SetTimeout(_camhandle, IS_TRIGGER_TIMEOUT, 100000));
    
    int onoff = 1;
    
    try{
    check_ueye_error(is_DeviceFeature(_camhandle,IS_DEVICE_FEATURE_CMD_SET_MEMORY_MODE_ENABLE, &onoff,sizeof(int)));
    }
    catch(std::exception& e)
    {
        _lg.Debug(e.what());
    }
    
    //Camera parameters
    std::tuple<int,int>  widthAndHeight = getImageSize();
    camWidth = std::get<0>(widthAndHeight);
    camHeight = std::get<1>(widthAndHeight);
    setColorMode(IS_CM_MONO8);
    camBitsperPixel = getBitsperPixel();
    exposure = getExposure();
    pixelClock = getPixelClock();
    frameRate = getFrameRate(); //it always returns 0
    camImage = Image(camWidth, camHeight, camBitsperPixel);
    iscaptured = false;
    firstTimeStamp = 0;
};


const std::string idscamera::getDeviceTypeName() const
{
  return "idscamera";
}

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

void idscamera::setColorMode(const int mode)
{
    check_ueye_error(is_SetColorMode(_camhandle,mode));
}

void idscamera::setPixelClock(double pclock)
{
    set_ueye_parameter_command(is_PixelClock, IS_PIXELCLOCK_CMD_SET, pclock);
    pixelClock = pclock;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

uint32_t idscamera::getPixelClock()
{
    return read_ueye_parameter_command<uint32_t>(is_PixelClock, IS_PIXELCLOCK_CMD_GET);
};

void idscamera::setFrameRate(double exp)
{
    double newFPS = 0;
    detail::check_ueye_error(is_SetFrameRate(_camhandle, exp, &newFPS), &_lg);
    frameRate = newFPS;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    _lg.Info("The frame rate has been changed to " + std::to_string(newFPS));
}

double idscamera::getFrameRate()  //It does not seem to work as expected
{
    double out = -1;
    detail::check_ueye_error(is_GetFramesPerSecond(_camhandle, &out), &_lg);
    return out;
}

void idscamera::setExposure(double exp)
{
    set_ueye_parameter_command(is_Exposure, IS_EXPOSURE_CMD_SET_EXPOSURE, exp);
    exposure = exp;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

double idscamera::getExposure() 
{
    return read_ueye_parameter_command<double>(is_Exposure, IS_EXPOSURE_CMD_GET_EXPOSURE);
}

int idscamera::getWidth()
{
    return camWidth;
}

int idscamera::getHeight()
{
    return camHeight;
}

char * idscamera::getLastUsedCapturePointer()
{
    char* pMem;
    check_ueye_error(is_GetImageMem(_camhandle, reinterpret_cast<void**>(&pMem)));
    
    return pMem;
}

void idscamera::waitEvent(unsigned timeout_ms, const int event)
{
    char * ppcMem;
    check_ueye_error(is_EnableEvent(_camhandle, event));
    
    int nRet = is_WaitEvent(_camhandle, event, timeout_ms);
    
    if (nRet == IS_SUCCESS)
    {
        _lg.Debug("Successfull waiting");
    }
    else if (nRet == IS_TIMED_OUT)
    {
        _lg.Debug("Time out reached");
    }
    else
    {
        _lg.Debug("Error waiting");
    }
    
    check_ueye_error(is_DisableEvent(_camhandle, event));
    
}

foxtrot::devices::Image idscamera::getSingleImage()
{
    
    _lg.Trace("getting single image");
    check_ueye_error(is_FreezeVideo(_camhandle,IS_WAIT));
    auto ptr = getLastUsedCapturePointer();
    auto im = _ring_buffered_images.at(ptr);
    Image imout(*im);
    
    return imout;
}

void idscamera::AddImageToSequence(std::shared_ptr<Image> image)
{
    int pid;
    
    auto imdat = reinterpret_cast<char*>(image->rawData.data());
    
    check_ueye_error(is_SetAllocatedImageMem(_camhandle, image->width, image->height, image->bitsperpixel, imdat, &pid));
    
    image->currentbufferid = pid;
    //image->_camera_backptr = this;
    
    _ring_buffered_images.insert( {imdat, std::move(image)});
    
    _lg.Trace("adding to ring buffer sequence:" + std::to_string(pid));
    check_ueye_error(is_AddToSequence(_camhandle,imdat,pid));
    
    
}


void idscamera::captureImage()
{
    auto imptr = &camImage;
    auto imdat = reinterpret_cast<char*>(imptr->rawData.data());
    int pid;
    if (iscaptured)
    {
        check_ueye_error(is_FreeImageMem(_camhandle, imdat, imptr->currentbufferid));
    }
    check_ueye_error(is_SetAllocatedImageMem(_camhandle, imptr->width, imptr->height, imptr->bitsperpixel, imdat, &pid));
    imptr->currentbufferid = pid;
    check_ueye_error(is_SetImageMem(_camhandle, imdat, imptr->currentbufferid));
    check_ueye_error(is_FreezeVideo(_camhandle,IS_WAIT));

    //Take the first timestamp of the program
    if (iscaptured == false)
    {
        UEYEIMAGEINFO infostruct;
        check_ueye_error( is_GetImageInfo(_camhandle,camImage.currentbufferid,&infostruct,sizeof(infostruct)));
        firstTimeStamp = infostruct.u64TimestampDevice / 10;
    }
    
    iscaptured = true;
}

foxtrot::devices::metadata idscamera::getImageMetadata()
{
    if (iscaptured == false)
    {
        _lg.Error("No image has been captured");
        throw DeviceError("No image has been captured");
    };
    
    foxtrot::devices::metadata image_meta;
    
    image_meta.width = camImage.getWidth();
    image_meta.height = camImage.getHeight();
    image_meta.bitsPerPixel = camImage.bitsperpixel;
    image_meta.exposure = exposure;
    image_meta.pixelClock = pixelClock;
    image_meta.frameRate = frameRate;
    image_meta.timestamp = getTimestamp() - firstTimeStamp;
    image_meta.dateCaptured = getDateCaptured();
    
    return image_meta;
    
}

std::vector<unsigned char> idscamera::getImageRawData()
{
    if (iscaptured == false)
    {
        _lg.Error("No image has been captured");
        throw DeviceError("No image has been captured");
    };
    
    std::vector<unsigned char> outvec;
    
    outvec.resize(camImage.datasize);
    std::copy(camImage.rawData.begin(), camImage.rawData.end(), outvec.begin());
    
    return outvec;
}


std::tuple<int, int> idscamera::getImageSize()
{
    IS_SIZE_2D param;
    check_ueye_error( is_AOI(_camhandle, IS_AOI_IMAGE_GET_SIZE, &param, sizeof(param)));
    
    return std::make_tuple(param.s32Width, param.s32Height);
    
}

int idscamera::getBitsperPixel()
{

    auto colmode = is_SetColorMode(_camhandle, IS_GET_COLOR_MODE);
    _lg.Trace("colmode: " + std::to_string(colmode));
    switch(colmode)
    {
        case(IS_CM_SENSOR_RAW8): 
        case(IS_CM_MONO8):
            return 8;
        case(IS_CM_SENSOR_RAW10):
        case(IS_CM_MONO10):
            return 16;
        
    }
    
    throw std::logic_error("unimplemented color mode in getBitsPixel");
    
}

unsigned int idscamera::getTimestamp()
{
    if (iscaptured == false)
    {
        _lg.Error("You must capture an image before accessing its timestamp");
        throw DeviceError("You must capture an image before accessing its timestamp");
    };
    
    UEYEIMAGEINFO infostruct;
    check_ueye_error( is_GetImageInfo(_camhandle,camImage.currentbufferid,&infostruct,sizeof(infostruct)));
    unsigned int timestamp = infostruct.u64TimestampDevice / 10;

    return timestamp;
}

date idscamera::getDateCaptured()
{
    if (iscaptured == false)
    {
        _lg.Error("You must capture an image before accessing the date");
        throw DeviceError("You must capture an image before accessing teh date");
    };
    
    foxtrot::devices::date dateCap;
    
    UEYEIMAGEINFO infostruct;
    check_ueye_error( is_GetImageInfo(_camhandle,camImage.currentbufferid,&infostruct,sizeof(infostruct)));
    auto systime = infostruct.TimestampSystem;
    
    dateCap.year = systime.wYear;
    dateCap.month = systime.wMonth;
    dateCap.day = systime.wDay;
    dateCap.hour = systime.wHour;
    dateCap.minute = systime.wMinute;
    dateCap.second = systime.wSecond;
    dateCap.millisecond = systime.wMilliseconds;

    return dateCap;
}


void idscamera::printoutImage()
{
    static int nfiles;
    nfiles++;
    std::ofstream outfile;
    outfile.open("imageprint_" + std::to_string(nfiles) + ".txt");
    int cnt = 0;
    for (int i = 0; i < camImage.datasize; i++)
    {
        cnt++;
        if (cnt % camImage.width == 0)
        {
            outfile << static_cast<unsigned>(camImage.rawData[i]) << "\n";
        } else 
        {
            outfile << static_cast<unsigned>(camImage.rawData[i]) << ", ";
        }
    }
    outfile.close();
}



RTTR_REGISTRATION{
    using namespace rttr;
    rttr::registration::class_<idscamera>("foxtrot::devices::idscamera")
    .constructor<const uint32_t* const>()
    
    //methods
    .method("getPixelClock", &idscamera::getPixelClock)
    .method("setPixelClock", &idscamera::setPixelClock)
    (parameter_names("pixel clock"))
    .method("getExposure", &idscamera::getExposure)
    .method("setExposure", &idscamera::setExposure)
    (parameter_names("exposure"))
    .method("getWidth", &idscamera::getWidth)
    .method("getHeight", &idscamera::getHeight)
    .method("captureImage", &idscamera::captureImage)
    .method("getImageMetadata", &idscamera::getImageMetadata)
    .method("getImageRawData", &idscamera::getImageRawData)
    .method("getFrameRate", &idscamera::getFrameRate)
    .method("setFrameRate", &idscamera::setFrameRate)
    (parameter_names("frame rate per second"))
    .method("setColorMode", &idscamera::setColorMode)
    .method("getBitsperPixel", &idscamera::getBitsperPixel);
    
    //Custom structs
    using foxtrot::devices::date;
    registration::class_<date>("foxtrot::devices::date")
    .constructor()(policy::ctor::as_object)
    .property("year", &date::year)
    .property("month", &date::month)
    .property("day", &date::day)
    .property("hour", &date::hour)
    .property("minute", &date::minute)
    .property("second", &date::second)
    .property("millisecond", &date::millisecond);

    
    using foxtrot::devices::metadata;
    registration::class_<metadata>("foxtrot::devices::metadata")
    .constructor()(policy::ctor::as_object)
    .property("width", &metadata::width)
    .property("height", &metadata::height)
    .property("bitsPerPixel", &metadata::bitsPerPixel)
    .property("exposure", &metadata::exposure)
    .property("pixelClock", &metadata::pixelClock)
    .property("frameRate", &metadata::frameRate)
    .property("timestamp", &metadata::timestamp)
    .property("dateCaptured", &metadata::dateCaptured);
    
}
