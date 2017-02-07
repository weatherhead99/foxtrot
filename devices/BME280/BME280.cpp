#include "BME280.h"
#include "DeviceError.h"

#define REG_DATA 0xF7
#define REG_CONTROL 0xF4
#define REG_CONFIG 0xF5

#define REG_CONTROL_HUM 0xF2
#define REG_HUM_MSB 0xFD
#define REG_HUM_LSB 0xFE



foxtrot::devices::BME280::BME280(std::shared_ptr<CommunicationProtocol> proto)
: Device(proto), _i2c_proto(std::dynamic_pointer_cast<foxtrot::protocols::i2c>(proto))
{
    if(_i2c_proto == nullptr)
    {
        throw foxtrot::DeviceError("couldn't cast protocol pointer to i2c");
    }
    
}

void foxtrot::devices::BME280::ReadCalibrationData()
{
    
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
    
    
}
