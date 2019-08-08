#pragma once

#include <vector>
#include "Image.h"
#include <foxtrot/server/Device.h>
#include <foxtrot/Logging.h>
#include <rttr/registration>

namespace foxtrot{
    namespace devices {
        
        namespace detail
        {
            void check_ueye_error(int32_t ret, foxtrot::Logging* lg=nullptr);
        }
        
        #pragma pack (push,1)
        struct metadata {
            int width;
            int height;
            int bitsPerPixel;
            double exposure;
            double pixelClock;
            double frameRate;
        };
        
        class idscamera : public Device
        {
            RTTR_ENABLE(Device);
        public:
            //constructors
            idscamera(const uint32_t* const camid=nullptr);
            
            //destructor
            ~idscamera();
            
            //methods
            uint32_t getPixelClock();
            void setPixelClock(double pclock);
            double getExposure();
            void setExposure(double exp);
            void setFrameRate(double exp);
            double getFrameRate();
            Image getSingleImage();
            metadata getImageMetadata(int camWidth, int camHeight, int camBitsperPixel);
            std::vector<int> getImageRawData(int camWidth, int camHeight, int camBitsperPixel);
            void getSingleImageAlone(std::shared_ptr<Image> image);
            void AddImageToSequence(std::shared_ptr<Image> image); 
            void printoutImage(std::shared_ptr<Image> image);
            void waitEvent(unsigned timeout_ms, const int event); //it only works on Linux
            std::tuple<int,int> getImageSize();
            void setColorMode(const int mode);
            int getBitsperPixel();
            
            //properties
            double exposure;
            double pixelClock;
            double frameRate;
            int camWidth;
            int camHeight;
            int camBitsperPixel;
            
        private:
            template<typename T, typename Callable>
            T read_ueye_parameter_command(Callable fun, uint32_t command)
            {
                T out;
                detail::check_ueye_error(
                fun(_camhandle, command, reinterpret_cast<void*>(&out), sizeof(out)),
                                         &_lg);
                return out;
            }
            
            template<typename T, typename Callable>
            void set_ueye_parameter_command(Callable fun, uint32_t command, const T& val)
            {
                detail::check_ueye_error(
                    fun(_camhandle, command, 
                        reinterpret_cast<void*>(const_cast<T*>(&val)), sizeof(val)), &_lg);
            };
            
            uint32_t _camhandle;
            foxtrot::Logging _lg;
            char * getLastUsedCapturePointer();
            std::map<const char*,std::shared_ptr<Image>> _ring_buffered_images;
        };
        
    }
}
