#include "BME280.h"
#include "DeviceError.h"

#include <iostream>

#define REG_DATA 0xF7
#define REG_CONTROL 0xF4
#define REG_CONFIG 0xF5

#define REG_CONTROL_HUM 0xF2
#define REG_HUM_MSB 0xFD
#define REG_HUM_LSB 0xFE


using std::cout;
using std::endl;

//most of this taken from https://bitbucket.org/MattHawkinsUK/rpispy-misc/raw/master/python/bme280.py

foxtrot::devices::BME280::BME280(std::shared_ptr<CommunicationProtocol> proto)
: Device(proto), _i2c_proto(std::dynamic_pointer_cast<foxtrot::protocols::i2c>(proto)),
_lg("BME280")
{
    if(_i2c_proto == nullptr)
    {
        throw foxtrot::DeviceError("couldn't cast protocol pointer to i2c");
    }

    _i2c_proto->Init(nullptr);
    SetupControlRegister(2,2,2,1);
    ReadCalibrationData();
    
}


void foxtrot::devices::BME280::SetupControlRegister(unsigned char osample_temp, unsigned char osample_pres, unsigned char osample_hum, unsigned char charmode)
{
  _i2c_proto->write_byte_data(REG_CONTROL_HUM, osample_hum);
  auto control = osample_temp << 5 | osample_pres << 2 | charmode;
  _i2c_proto->write_byte_data(REG_CONTROL, control);
  
}


void foxtrot::devices::BME280::ReadCalibrationData()
{
    
  auto cal1 = _i2c_proto->read_block_data(0x88,24);
  auto cal2 = _i2c_proto->read_block_data(0xA1,1);
  auto cal3 = _i2c_proto->read_block_data(0xE1,6);
  _lg.Debug("read calibration constants");


  for(auto& c : cal1)
    {
      cout << (int) c << '\t' ;
    }
	cout << endl;

		cout << "----" << endl;

				  for(auto& c : cal2)
				    {
				      cout << (int) c << '\t';
				    }
					cout << endl;
		
	      
  //clench...
  caldata_struct* interpreted_caldata =  reinterpret_cast<caldata_struct*>(cal1.data());
		if(interpreted_caldata == nullptr)
		  {
		    throw std::logic_error("invalid caldata struct assignment");

		  }
  _caldata = *interpreted_caldata;

		_lg.Debug("copied caldata");
		_H1 = cal2[0];
		_lg.Debug("copied cal2");
  
		
  humidity_caldata* interpreted_humdata = reinterpret_cast<humidity_caldata*>(cal3.data());
		if(interpreted_humdata == nullptr)
		  {

		    throw std::logic_error("invalid humdata struct assignment");
		  }
  _humcaldata = *interpreted_humdata;
		_lg.Debug("copied cal3");
		
  
  _humcaldata.H4 <<= 4;
  _humcaldata.H4 |= (_humcaldata.H4a & 0x0F);
  
  _humcaldata.H5 <<= 4;
  _humcaldata.H5 |= (_humcaldata.H4a >> 4 & 0x0F);
		
		
  _wait_time_ms = 1.25 + (2.3 * _oversample_temp) + ((2.3*_oversample_pres) +0.575) + ((2.3*_oversample_hum)+0.575 ) ;
  
}

void foxtrot::devices::BME280::ReadData()
{
    //read raw data
    auto data = _i2c_proto->read_block_data(REG_DATA,8);
    _pressure_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >>4);
    _temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    _humidity_raw = (data[6] << 8) | data[7];
    
    //refine temperature
    auto var1 = ((((_temp_raw>>3)-(_caldata.T1<<1)))*(_caldata.T2)) >> 11;
    auto var2 = (((((_temp_raw>>4) - (_caldata.T1)) * ((_temp_raw>>4) - (_caldata.T1))) >> 12) * (_caldata.T3)) >> 14;
    auto t_fine = var1+var2;
    
    _temperature =  ((t_fine *5) + 128) >> 8;
    
    // Refine pressure and adjust for temperature
    var1 = t_fine / 2.0 - 64000.0;
    var2 = var1 * var1 * _caldata.P6 / 32768.0;
    var2 = var2 + var1 * _caldata.P5 * 2.0;
    var2 = var2 / 4.0 + _caldata.P4 * 65536.0;
    var1 = (_caldata.P3 * var1 * var1 / 524288.0 + _caldata.P2 * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * _caldata.P1;
    
    if(var1 == 0)
    {
      _pressure=0.;
    }
    else
    {
      _pressure = 1048576.0 - _pressure_raw;
      _pressure = ((_pressure - var2 / 4096.0) * 6250.0) / var1;
      var1 = _caldata.P9 * _pressure * _pressure / 2147483648.0;
      var2 = _pressure * _caldata.P8 / 32768.0;
      _pressure += (var1 + var2 + _caldata.P7) / 16.0;

    }
    // Refine humidity
    _humidity = t_fine - 76800.0;
    _humidity = (_humidity_raw - (_humcaldata.H4 * 64.0 + _humcaldata.H5/ 16384.0 * _humidity)) *
    (_humcaldata.H2 / 65536.0 * (1.0 + _humcaldata.H6 / 67108864.0 * _humidity * (1.0 + _humcaldata.H3/ 67108864.0 * _humidity)));
    _humidity *= (1.0 - _H1 * _humidity / 524288.0);
    
    if(_humidity > 100)
    {
      _humidity = 100;
    }
    else if(_humidity < 0)
    {
      _humidity = 0.;
    };

    _temperature /= 100.;
    _pressure /= 100.;
    
}

double foxtrot::devices::BME280::GetHumidity_pc()
{
    return _humidity;
}

double foxtrot::devices::BME280::GetPressure_hPa()
{
    return _pressure;
}


double foxtrot::devices::BME280::GetTemperature_C()
{
    return _temperature;
}


//adapted from https://github.com/BoschSensortec/BME280_driver
double foxtrot::devices::BME280::compensate_temperature(int raw_temp, int& t_fine)
{
  unsigned x1 =  ((raw_temp >> 3) - (_caldata.T1 << 1)) * (_caldata.T2 >>11);
  unsigned x2 = (((((raw_temp >> 4) - (_caldata.T1)) * 
  ((raw_temp>> 4) - (_caldata.T1))) >> 12) * (_caldata.T3)) >> 14;
  
  t_fine = x1 + x2;
  
  auto temp = (t_fine * 5 + 128) >> 8;
  return temp / 100.;
  
}

double foxtrot::devices::BME280::compensate_pressure(int raw_pressure, int t_fine)
{
  unsigned x1 = (t_fine >> 1) - 64000;
  unsigned x2 = (((x1 >> 2) * (x1 >>2)) >> 11) * _caldata.P6;

  x2 += ((x1 * (_caldata.P5)) << 1);
  x2 = (x2 >> 2) + ((_caldata.P4) << 16);
  
  x1 = (((_caldata.P3 * (((x1 >> 2) * (x1 >>2)) >>13 )) >> 3) + (((_caldata.P2) * x1) >> 1)) >> 18;
  x1 = ((32768 + x1) *(_caldata.P1)) >> 15;
  
  
  
}




RTTR_REGISTRATION{
 using namespace rttr;
 using foxtrot::devices::BME280;
 
 registration::class_<BME280>("foxtrot::devices::BME280")
 .property_readonly("GetTemperature_C",&BME280::GetTemperature_C)
 .property_readonly("GetPressure_hPa",&BME280::GetPressure_hPa)
 .property_readonly("getHumidity_pc",&BME280::GetHumidity_pc)
 .method("ReadData",&BME280::ReadData)
 ;
    
    
}
