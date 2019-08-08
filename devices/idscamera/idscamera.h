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
            unsigned long long time_stamp_mus;
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
            const std::string getDeviceTypeName() const override;
            uint32_t getPixelClock();
            void setPixelClock(double pclock);
            double getExposure();
            void setExposure(double exp);
            void setFrameRate(double exp);
            double getFrameRate();
            int getWidth();
            int getHeight();
            int getBitsperPixel();
            Image getSingleImage();
            void captureImage();
            metadata getImageMetadata();
            std::vector<unsigned char> getImageRawData();
            void AddImageToSequence(std::shared_ptr<Image> image); 
            void printoutImage();
            void waitEvent(unsigned timeout_ms, const int event); //it only works on Linux
            std::tuple<int,int> getImageSize();
            void setColorMode(const int mode);
  
        private:
            //templates
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
            
            //properties
            bool iscaptured;
            uint32_t _camhandle;
            foxtrot::Logging _lg;
            char * getLastUsedCapturePointer();
            std::map<const char*,std::shared_ptr<Image>> _ring_buffered_images;
            Image camImage;
            double exposure;
            double pixelClock;
            double frameRate;
            int camWidth;
            int camHeight;
            int camBitsperPixel;
            
        };
        
    }
}
