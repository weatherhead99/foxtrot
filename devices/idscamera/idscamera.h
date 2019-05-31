#pragma once

#include <foxtrot/server/Device.h>
#include <foxtrot/Logging.h>
#include <rttr/registration>

namespace foxtrot{
    namespace devices {
        
        namespace detail
        {
            void check_ueye_error(int32_t ret, foxtrot::Logging* lg=nullptr);
        }
        
        class idscamera : public Device
        {
            RTTR_ENABLE(Device);
        public:
            idscamera(const uint32_t* const camid=nullptr);
            ~idscamera();
            uint32_t getPixelClock();
            void setPixelClock(uint32_t pclock);
            double getExposure();
            void setExposure(double exp);
            
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
                        reinterpret_cast<void*>(const_cast<T*>(&val)), sizeof(val)),                                  &_lg);
            };
            
            uint32_t _camhandle;
            foxtrot::Logging _lg;
        };
        
    }
}
