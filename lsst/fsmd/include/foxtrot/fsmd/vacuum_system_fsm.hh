#pragma once
#include <tinyfsm.hpp>
#include <foxtrot/Logging.h>
#include <foxtrot/fsmd/common_events.hh>

//transition events
namespace foxtrot{
namespace fsmd {

class VacuumSystemFSM : public tinyfsm::Fsm<VacuumSystemFSM>
{
public:
    void react(const tinyfsm::Event& ev);
    virtual void react(DataUpdateEvent& ev);
    virtual void react(TimeTickEvent& ev);
    
    virtual void entry() = 0;
    virtual void exit() = 0;
private:
    foxtrot::Logging _lg{"VacuumSystemFSM"};
};

class InitialVacuumState : public VacuumSystemFSM 
{
    void react(DataUpdateEvent & ev) override;
};


class HighVibrationOperation : public VacuumSystemFSM
{

};

class PumpSpinningDown : public VacuumSystemFSM
{

};

class PumpVenting : public VacuumSystemFSM
{
    
};

class PumpedToAtmosphere : public VacuumSystemFSM
{

};

class PumpSpinningUp : public VacuumSystemFSM
{

};

class TopUpNeeded : public VacuumSystemFSM
{
    
};

class LowVibrationRunning : public VacuumSystemFSM
{
    
};

class CryostatPumpDown : public VacuumSystemFSM
{
    
};

class EmergencyBailout : public VacuumSystemFSM
{
    
};

}
}
