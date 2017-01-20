#pragma once
#include "archon_modules.h"
#include <array>

namespace foxtrot
{
namespace devices
{
    class ArchonHeaterX : public ArchonModule
    {
    public:
    double getHeaterAOutput() const;
    double getHeaterBOutput() const;
    double getTempA() const;
    double getTempB() const;
    double getTempC() const;
    std::array<bool,8> getGPIO() const;  
    
    private:
    ArchonHeaterX(archon& arch, short unsigned int modpos);
    virtual void update_variables() override;
    
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
        
    };
        
}
}
