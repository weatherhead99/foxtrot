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
    VacuumSystemFSM(double pumpdown_thresh, double ambient_thresh, double lowvibration_thresh);
    VacuumSystemFSM() {};
    
    virtual void react(DataUpdateEvent& ev) {};
    virtual void react(TimeTickEvent& ev) {};
    virtual std::string name() const = 0;
    
    virtual void entry()  {};
    virtual void exit()  {};
protected:
    foxtrot::Logging _lg{"VacuumSystemFSM"};
    static double _pumpdown_thresh;
    static double _ambient_thresh; 
    static double _pump_lowvibration_rot_thresh;
    static bool _instance_created;
private:
};

class InitialVacuumState : public VacuumSystemFSM 
{
    void react(DataUpdateEvent & ev) override;
    std::string name() const override;
};


class HighVibrationOperation : public VacuumSystemFSM
{
    std::string name() const override;

};

class PumpSpinningDown : public VacuumSystemFSM
{
    std::string name() const override;
};

class PumpVenting : public VacuumSystemFSM
{
    std::string name() const override;
};

class PumpedToAtmosphere : public VacuumSystemFSM
{
    std::string name() const override;

};

class PumpSpinningUp : public VacuumSystemFSM
{

};

class TopUpNeeded : public VacuumSystemFSM
{
    
};

class LowVibrationRunning : public VacuumSystemFSM
{
    std::string name() const override;
};

class CryostatPumpDown : public VacuumSystemFSM
{
    std::string name() const override;
    
};

class EmergencyBailout : public VacuumSystemFSM
{
    
};

}
}
