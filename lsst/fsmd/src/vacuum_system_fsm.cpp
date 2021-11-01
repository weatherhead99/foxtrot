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

void InitialVacuumState::react(DataUpdateEvent& ev)
{
    _lg.strm(sl::debug) << "initial vacuum state started, trying to determine current operation status";
    
    
    //cases where the valve is open and pump is disabled
    if(!ev.pump_enabled && ev.valve_open && (ev.pump_pressure > _ambient_thresh) )
    {
        _lg.strm(sl::info) << "conditions appear to indicate we are at ambient";
        transit<PumpedToAtmosphere>();    
    }
    else if(!ev.pump_enabled  && ev.valve_open)
    {
        _lg.strm(sl::info) << "conditions indicate we are venting to ambient";
        transit<PumpVenting>();
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
    
    if(!ev.pump_enabled && !ev.valve_open && (ev.pump_rot_speed > _pump_lowvibration_rot_thresh))
    {
        _lg.strm(sl::info) << "conditions indicate the pump is spinning down";
        transit<PumpSpinningDown>();
    }
    else if(!ev.pump_enabled && !ev.valve_open)
    {
        _lg.strm(sl::info) << "conditions indicate we are in low vibration running conditions";
        transit<LowVibrationRunning>();
    }
    
    
    
    
    
}


std::string HighVibrationOperation::name() const { return "HighVibrationOperation";};
std::string PumpSpinningDown::name() const { return "PumpSpinningDown";};
std::string PumpVenting::name() const { return "Pumpventing";};


std::string PumpedToAtmosphere::name() const { return "PumpedToAtmosphere";}
std::string LowVibrationRunning::name() const {return "LowVibrationRunning";}
std::string CryostatPumpDown::name() const { return "CryostatPumpDown";}
