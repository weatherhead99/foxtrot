#pragma once	
#include "archon_modules.h"
#include <array>



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
  
    class ArchonHeaterX : public ArchonModule
    {
      friend class archon;
      
      
    public:
    static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
        
    double getHeaterAOutput() const;
    double getHeaterBOutput() const;
    double getHeaterAP() const;
    double getHeaterAI() const;
    double getHeaterAD() const;
    
    double getHeaterBP() const;
    double getHeaterBI() const;
    double getHeaterBD() const;
    
    
    double getTempA() const;
    double getTempB() const;
    double getTempC() const;
    std::array<bool,8> getGPIO() const;  
    
    void setSensorType(HeaterXSensors sensor, HeaterXSensorTypes type) ;
    HeaterXSensorTypes getSensorType(HeaterXSensors sensor);
    
    void setSensorCurrent(HeaterXSensors sensor, int curr_na);
    int getSensorCurrent(HeaterXSensors sensor);
    
    
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
    
    double _heaterAP;
    double _heaterAI;
    double _heaterAD;
    
    double _heaterBP;
    double _heaterBI;
    double _heaterBD;
    
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
