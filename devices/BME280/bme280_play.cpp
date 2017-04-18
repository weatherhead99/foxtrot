#include <iostream>
#include <backward.hpp>
#include "i2c.h"
#include "BME280.h"

using std::cout;
using std::endl;

const foxtrot::parameterset i2c_params
{
    {"devnode", "/dev/i2c-1"},
    {"address", 76u}
};


int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    
    
    auto proto = std::make_shared<foxtrot::protocols::i2c>(&i2c_params);
    foxtrot::devices::BME280 sensor(proto);
    
    sensor.ReadData();
    
    auto temp = sensor.GetTemperature_C();
    auto pres = sensor.GetPressure_hPa();
    auto hum = sensor.GetHumidity_pc();

    cout << "temp: " << temp << endl;
    cout << "pres: " << pres << endl;
    cout << "hum: " << hum << endl;
    

}
