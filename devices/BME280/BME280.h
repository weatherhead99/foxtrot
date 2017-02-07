#pragma once

#include <memory>
#include "Device.h"
#include "CommunicationProtocol.h"
#include "i2c.h"

namespace foxtrot
{
namespace devices
{
 class BME280 : public Device 
 {
 public:
     BME280(std::shared_ptr<CommunicationProtocol> proto);
     void ReadData();
     
 private:
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
     
     
     struct humidity_caldata
     {
       short H2;
       unsigned char H3;
       char H4;
       char H5;
       char H6;
     };
     
     
     
     void ReadCalibrationData();
     
     unsigned char _oversample_temp;
     unsigned char _oversample_pres;
     unsigned char _mode;
     
     int _pressure_raw;
     int _temp_raw;
     int _humidity_raw;
     
     double _temperature;
     double _pressure;
     double _humidity;
     
     caldata_struct _caldata;
     humidity_caldata _humcaldata;
     unsigned char _H1;
     
     std::shared_ptr<foxtrot::protocols::i2c> _i2c_proto;
     
 };
    
}
    

}
