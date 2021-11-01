#include <foxtrot/fsmd/cryo_system_fsm.hh>
#include <foxtrot/fsmd/fsm_io.hh>

using namespace foxtrot::fsmd;


double CryoSystemFSM::_cryo_empty_temp_thresh = 0.0;
double CryoSystemFSM::_allowed_pressure_thresh = 0.0;
duration_s CryoSystemFSM::_fill_time_preset = std::chrono::seconds{0};
bool CryoSystemFSM::_instance_created = false;



CryoSystemFSM::CryoSystemFSM(double cryo_empty_temp_thresh, 
                             double allowed_pressure_thresh,
                             duration_s fill_time_preset)
{
    _cryo_empty_temp_thresh = cryo_empty_temp_thresh;
    _allowed_pressure_thresh = allowed_pressure_thresh;
    _fill_time_preset = fill_time_preset;
    _instance_created = true;
}


bool CryoSystemFSM::should_refill ( const DataUpdateEvent& ev )
{
    if(ev.tank_temp < _cryo_empty_temp_thresh)
    {
        _lg.strm(sl::debug) << "cryo appears to be full, no need to refill..";
        return false;
    }
    else if(ev.autofill_enabled && (ev.cryo_pressure < _allowed_pressure_thresh))
    {
        _lg.strm(sl::debug) << "cryo empty and pressure within limits, allowing refill";
        return true;
    }
    else if(ev.cryo_pressure > _allowed_pressure_thresh)
    {
        _lg.strm(sl::debug) << "cryo empty but pressure too high, no refill allowed";
        return false;
    }
    _lg.strm(sl::debug) << "autofill disabled, no refill";
    return false;
    
}


void CryoSystemFSM::entry()
{
    _lg.strm(sl::trace) << "changing FSM state flags";
    auto now = std::chrono::system_clock::now();
    //TODO: pass through client here somehow!
    update_fsm_state_flags(nullptr, "CryoSystem", name(), now);
}

std::string InitialCryoState::name() const { return "InitiialCryoState";};

void InitialCryoState::react(const DataUpdateEvent& ev)
{
    //if tank is full goto full state
    if( ev.tank_temp < _cryo_empty_temp_thresh )
    {
        _lg.strm(sl::info) << "cryo appears to be full...";
        transit<CryostatFull>();
    }
    //if tank is empty and autofill enabled, go to filling state
    else if(should_refill(ev))
      {
        _lg.strm(sl::info) << "cryo appears to be empty, attempting refill..";
        transit<CryostatFilling>();
      }
    else
    {
        _lg.strm(sl::info) << "cryo appears to be empty, no refill required, leaving empty...";
        transit<CryostatEmpty>();
    }
    
};

std::string CryostatEmpty::name() const { return "CryoStatEmpty";};

void CryostatEmpty::react(const DataUpdateEvent& ev)
{
    if(should_refill(ev))
       {
           _lg.strm(sl::debug) << "cryostat empty and needs refill, transitioning to fill state...";
           transit<CryostatFilling>();
       }
       
}

std::string CryostatFull::name() const
{
    return "CryostatFull";
}

void CryostatFull::react ( const foxtrot::fsmd::DataUpdateEvent& ev )
{
    if(ev.tank_temp > _cryo_empty_temp_thresh)
    {
        _lg.strm(sl::info) << "cryostat appears to have become empty...";
        transit<CryostatEmpty>();
    }
    
}


std::string CryostatFilling::name() const
{
    return "CryostatFilling";
}


void CryostatFilling::entry()
{
    _lg.strm(sl::trace) << "setting cryostat fill start tiime... " ;
    _start_time = std::chrono::system_clock::now();
}


void CryostatFilling::react(const TimeTickEvent& ev)
{
    _lg.strm(sl::trace) << "time tick, updating fill time...";
    auto now = std::chrono::system_clock::now();
    _filling_time = std::chrono::duration_cast<std::chrono::seconds>(now - _start_time);
    
}

void CryostatFilling::react(const DataUpdateEvent& ev)
{
    if(_filling_time > _fill_time_preset)
    {
        _lg.strm(sl::info) << "filling time exceeded, checking whether cryostat is full...";
        if(ev.tank_temp < _cryo_empty_temp_thresh)
        {
            _lg.strm(sl::info) << "filling succeeded, transitioning to full state...";
            transit<CryostatFull>();
        }
        else
        {
            _lg.strm(sl::info) << "filling seems to have failed...";
            transit<TankEmpty>();
        }
        
    }
    
}



std::string foxtrot::fsmd::TankEmpty::name() const
{
    return "TankEmpty";
}


void TankEmpty::react ( const DataUpdateEvent& ev )
{
    if(ev.tank_refill_flag_set)
    {
        _lg.strm(sl::info) << "tank refill flag set";
        transit<TankRefilled>();
    }
    
}


std::string TankRefilled::name() const
{
    return "TankRefilled";
}


void TankRefilled::react ( const DataUpdateEvent& ev )
{
    if(should_refill(ev))
    {
        _lg.strm(sl::info) << " tank refilled and refil flag set, proceeding to fill cryostat";
        transit<CryostatFilling>();
    }
    
}





