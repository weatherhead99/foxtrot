#pragma once	
#include "archon_modules.h"
#include "archon_GPIO.h"
#include <array>
#include <rttr/registration>

namespace foxtrot
{
namespace devices
{
     enum class HeaterXSensorTypes : short unsigned {
       DT_670 = 0,
       DT_470 = 1,
       RTD100 = 2,
       RTD1000 = 3
     };
     
     enum class HeaterXSensors : char {
       A =  'A',
       B = 'B',
       C = 'C'
     };
     
     enum class HeaterXHeaters : char {
       A = 'A',
       B = 'B'
     };
  
    class ArchonHeaterX : public ArchonModule, public archonGPIO
    {
    RTTR_ENABLE(ArchonModule,archonGPIO)
      friend class archon;

    virtual const string getDeviceTypeName() const;
      
    public:
    static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
        
    double getHeaterAOutput() const;
    double getHeaterBOutput() const;
    int getHeaterAP() const;
    int getHeaterAI() const;
    int getHeaterAD() const;
    
    int getHeaterBP() const;
    int getHeaterBI() const;
    int getHeaterBD() const;
    
    
    double getTempA() const;
    double getTempB() const;
    double getTempC() const;
    std::array<bool,8> getGPIO() const;  
    
    void setSensorType(HeaterXSensors sensor, HeaterXSensorTypes type) ;
    HeaterXSensorTypes getSensorType(HeaterXSensors sensor);
    
    void setSensorCurrent(HeaterXSensors sensor, int curr_na);
    int getSensorCurrent(HeaterXSensors sensor);
    
    void setHeaterTarget(HeaterXHeaters heater, double target);
    double getHeaterTarget(HeaterXHeaters heater);
    
    void setHeaterSensor(HeaterXHeaters heater, HeaterXSensors sensor);
    HeaterXSensors getHeaterSensor(HeaterXHeaters heater);
    
    void setSensorLowerLimit(HeaterXSensors sensor, double temp);
    double getSensorLowerLimit(HeaterXSensors sensor);
    
    void setSensorUpperLimit(HeaterXSensors sensor, double temp);
    double getSensorUpperLimit(HeaterXSensors sensor);
    
    void setHeaterEnable(HeaterXHeaters heater, bool onoff);
    bool getHeaterEnable(HeaterXHeaters heater);
    
    void setHeaterLimit(HeaterXHeaters heater, double lim);
    double getHeaterLimit(HeaterXHeaters heater);
    
    
    //TODO: value validation on all of these!
    void setHeaterP(HeaterXHeaters heater, int p);
    int getHeaterP(HeaterXHeaters heater);
    
    void setHeaterI(HeaterXHeaters heater, int i);
    int getHeaterI(HeaterXHeaters heater);
    
    void setHeaterIL(HeaterXHeaters heater, int il);
    int getHeaterIL(HeaterXHeaters heater);
    
    void setHeaterD(HeaterXHeaters heater, int d);
    int getHeaterD(HeaterXHeaters heater);
    
    void setHeaterUpdateTime(int ms);
    int getHeaterUpdateTime();
    
    void setHeaterRamp(HeaterXHeaters heater, bool onoff);
    bool getHeaterRamp(HeaterXHeaters heater);
    
    void setHeaterRampRate(HeaterXHeaters heater, int rate_mk_ut);
    int getHeaterRampRate(HeaterXHeaters heater);
    
    void setHeaterLabel(HeaterXHeaters heater, const string& label);
    string getHeaterLabel(HeaterXHeaters heater);
    
    void setSensorLabel(HeaterXSensors sensor, const string& label);
    string getSensorLabel(HeaterXSensors sensor);
    
    
    virtual const string getTypeName() const;	
    
    private:
    virtual void update_variables() override;
    ArchonHeaterX(archon& arch, short unsigned int modpos);
    
    
    //read values
    double _heaterAOutput;
    double _heaterBOutput;
    double _TempA;
    double _TempB;
    double _TempC;
    
    int _heaterAP;
    int _heaterAI;
    int _heaterAD;
    
    int _heaterBP;
    int _heaterBI;
    int _heaterBD;
    
    std::array<bool,8> _GPIO;
        
    
    //write values
    bool _heaterAEnable;
    double _heaterATarget;
    double _heaterALimit;
    int _heaterASensor;
    int _heaterAPset;
    int _heaterAIset;
    int _heaterADset;
    
    
    
    
    };
    
    
    
        
}
}
