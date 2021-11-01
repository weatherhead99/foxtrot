#include <foxtrot/fsmd/vacuum_system_fsm.hh>

using namespace foxtrot::fsmd;


double VacuumSystemFSM::_pumpdown_thresh = 0.0;
double VacuumSystemFSM::_ambient_thresh = 0.0;
double VacuumSystemFSM::_pump_lowvibration_rot_thresh = 0.0;
bool VacuumSystemFSM::_instance_created = false;


foxtrot::fsmd::VacuumSystemFSM::VacuumSystemFSM(double pumpdown_thresh, double ambient_thresh, double lowvibration_thresh) 
{
    _pumpdown_thresh = pumpdown_thresh;
    _ambient_thresh = ambient_thresh;
    _pump_lowvibration_rot_thresh = lowvibration_thresh;
    _instance_created = true;
};


std::string InitialVacuumState::name() const { return "InitialVacuumState";}

void InitialVacuumState::react(const DataUpdateEvent& ev)
{
    _lg.strm(sl::debug) << "initial vacuum state started, trying to determine current operation status";
    
    
    //cases where the valve is open and pump is disabled
    if(!ev.pump_enabled && ev.valve_open && (ev.pump_pressure > _ambient_thresh) )
    {
        _lg.strm(sl::info) << "conditions appear to indicate we are at ambient";
        transit<PumpedToAtmosphere>();    
    }
    
    if(ev.pump_enabled && ev.valve_open && (ev.cryo_pressure > _pumpdown_thresh))
    {
        _lg.strm(sl::info) << "conditions indicate we are pumping down towards threshold";
        transit<CryostatPumpDown>();
        
    }
    else if(ev.pump_enabled && ev.valve_open)
    {
        _lg.strm(sl::info) << "conditions indicate we are at high vibration running conditions";
        transit<HighVibrationOperation>();
    }
    
    throw std::logic_error("cannot transition to a valid operation state!");
    
    
    
}


std::string HighVibrationOperation::name() const { return "HighVibrationOperation";};
std::string PumpSpinningDown::name() const { return "PumpSpinningDown";};
std::string PumpVenting::name() const { return "Pumpventing";};
void PumpVenting::react(const DataUpdateEvent& ev)
{
    if(!ev.pump_enabled && (ev.cryo_pressure > _ambient_thresh))
    {
        _lg.strm(sl::info) << "system has reached ambient pressure";
    }
    transit<PumpedToAtmosphere>();
    
    
    
    
}


std::string PumpedToAtmosphere::name() const { return "PumpedToAtmosphere";}
std::string LowVibrationRunning::name() const {return "LowVibrationRunning";}

std::string CryostatPumpDown::name() const { return "CryostatPumpDown";}
void CryostatPumpDown::react(const DataUpdateEvent& ev)
{
    if(ev.pump_enabled && (ev.cryo_pressure < _pumpdown_thresh) && !ev.lowvibration_requested)
    {
        _lg.strm(sl::info) << "we have reached pumped down status, transitioning to high vibration running";
        transit<HighVibrationOperation>();
    }
    else if(ev.pump_enabled && (ev.cryo_pressure < _pumpdown_thresh))
    {
        _lg.strm(sl::info) << "we have reached pumped down status, low vibration operation requested, spinning down";
        transit<PumpSpinningDown>();
    }
    
}

