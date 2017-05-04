#pragma once

#include <memory>
#include "Device.h"
#include "CommunicationProtocol.h"
#include "i2c.h"
#include <rttr/registration>
#include "Logging.h"

namespace foxtrot
{
namespace devices
{
 class BME280 : public Device 
 {
     RTTR_ENABLE(Device)
 public:
     BME280(std::shared_ptr<CommunicationProtocol> proto);
     double GetTemperature_C();
     double GetPressure_hPa();
     double GetHumidity_pc();
     
     void ReadData();
     
 private:
#pragma pack(push,1)
     struct caldata_struct
     {
         unsigned short T1;
         short T2;
         short T3;
         
         unsigned short P1;
         short P2;
         short P3;
         short P4;
         short P5;
         short P6;
         short P7;
         short P8;
         short P9;
         
     };
#pragma pack(pop)

#pragma pack(push,1)
     struct humidity_caldata
     {
       short H2;
       unsigned char H3;
       char H4;
       char H5;
       char H6;
     };
#pragma pack(pop)
     
     
     
     void ReadCalibrationData();
     void SetupControlRegister(unsigned char osample_temp, unsigned char osample_pres,unsigned char osample_hum, unsigned char mode);
     
     
     unsigned char _oversample_temp = 2;
     unsigned char _oversample_pres =2;
     unsigned char _oversample_hum = 2;
     unsigned char _mode = 1;
     
     int _pressure_raw;
     int _temp_raw;
     int _humidity_raw;
     
     double _temperature;
     double _pressure;
     double _humidity;
     
     caldata_struct _caldata;
     humidity_caldata _humcaldata;
     unsigned char _H1;
     
     float _wait_time_ms;
     
     foxtrot::Logging _lg;
     
     std::shared_ptr<foxtrot::protocols::i2c> _i2c_proto;
     
 };
    
}
    

}
