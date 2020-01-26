#include <malloc.h>
#include <ueye.h>
#include "Image.h"
#include <foxtrot/DeviceError.h>
#include <foxtrot/Logging.h>

//linux specific
#ifdef __linux__
#include <sys/mman.h>
#elif _WIN32
#include <Windows.h>
#endif

using namespace foxtrot;
using namespace foxtrot::devices;

using std::cout;
using std::endl;



//Image functions
Image::Image(const foxtrot::devices::Image& copyim): _lg("Image")
{
    width = copyim.width;
    height = copyim.height;
    bitsperpixel = copyim.bitsperpixel;
    currentbufferid = copyim.currentbufferid;
    datasize = copyim.datasize;
    rawData.resize(datasize);
    std::copy(copyim.rawData.begin(), copyim.rawData.end(), rawData.begin());

    lockMemory();

}

Image::Image(int width, int height, int bitsperpixel): _lg("Image")
{
    this->width = width;
    this->height = height;
    this->bitsperpixel = bitsperpixel;

    unsigned bytes_per_pixel = (bitsperpixel+7)/8;
    unsigned linelength = width * bytes_per_pixel;
    //WARNING: not sure if right
    unsigned adjust = (linelength % 4) ? (4 - linelength%4): 0;

    datasize = (width * bytes_per_pixel + adjust) * height;

    rawData.resize(datasize);

    lockMemory();

}

Image::Image(): _lg("Image")
{
    width = 1280;
    height = 1024;
    bitsperpixel = 8;

    unsigned bytes_per_pixel = (bitsperpixel+7)/8;
    unsigned linelength = width * bytes_per_pixel;
    unsigned adjust = (linelength % 4) ? (4 - linelength%4): 0;

    datasize = (width * bytes_per_pixel + adjust) * height;
    rawData.resize(datasize);

    lockMemory();
}

Image::~Image()
{
    unlockMemory();
}

int Image::getWidth() const
{
    return width;
}

int Image::getHeight() const
{
    return height;
}


void Image::lockMemory()
{

#ifdef __linux__
    auto ret = mlock(getRawDataPointer(), rawData.size());

    if(ret)
    {
        _lg.Error("error locking memory");
        throw std::runtime_error(strerror(errno));
    } else 
    {
        _lg.Info("memory locked successfully");
    }

#elif _WIN32
    SIZE_T dwMin, dwMax;
    auto sc_get = GetProcessWorkingSetSize(GetCurrentProcess(),&dwMin, &dwMax);
    std::cerr << "dwMax: " << dwMax << std::endl;

    auto ret = VirtualLock(rawData.data(), rawData.size());
    auto lasterror = GetLastError();
    std::cout << "ret: " << (int) ret << std::endl;
    if(ret == 0)
    {
        _lg.Error("error locking memory");

        std::cout << "GetLastError(): " << lasterror << std::endl;
        char* msgstr;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, lasterror,
                      MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPSTR) &msgstr, 0 , nullptr);

        throw std::runtime_error(msgstr);
    } else
    {
        _lg.Info("memory locked successfully");
    }
#endif

    lockedMemory = true;

}

char * Image::getRawDataPointer()
{
    return reinterpret_cast<char*>(rawData.data());
}

void Image::unlockMemory()
{

    //auto fallback_lg = ueyecc_general_logging::get();

#ifdef __linux__
    if(lockedMemory)
    {

        auto ret = munlock(getRawDataPointer() ,rawData.size());
        if(ret)
        {
            _lg.Error("error unlocking memory");
            throw std::runtime_error("error unlocking memory");
        }
        else
        {
            _lg.Info("memory unlocked successfully..");
        }

    }
#elif _WIN32
    auto ret = !VirtualUnlock(getRawDataPointer(),rawData.size());
    if(!ret)
    {
        _lg.Error("error unlocking memory");
        throw std::runtime_error("error unlocking memory");
    }
    else
    {
        _lg.Info("memory unlocked successfully..");
    }
#endif

}


RTTR_REGISTRATION{
    using namespace rttr;
    using foxtrot::devices::Image;
    registration::class_<Image>("foxtrot::devices::Image")
    
    //properties
    .property("width", &Image::width)
    .property("height", &Image::height)
    .property("bitsperpixel", &Image::bitsperpixel)
    .property("datasize", &Image::datasize)
    .property("rawData", &Image::rawData)
    
    //methods
    .method("getWidth", &Image::getWidth)
    .method("getHeight", &Image::getHeight);
    
}


