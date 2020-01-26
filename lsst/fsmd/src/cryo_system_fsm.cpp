#include <foxtrot/fsmd/cryo_system_fsm.hh>

using namespace foxtrot::fsmd;

CryoSystemFSM::CryoSystemFSM(double cryo_empty_temp_thresh, 
                             double allowed_pressure_thresh,
                             int fill_length_ticks)
{
    _cryo_empty_temp_thresh = cryo_empty_temp_thresh;
    _allowed_pressure_thresh = allowed_pressure_thresh;
    _fill_length_ticks = fill_length_ticks;
    _instance_created = true;
}

CryoSystemFSM::CryoSystemFSM()
{
    if(!_instance_created)
        throw std::logic_error("instance not created!");
    
}

void IntialCryoState::react(const DataUpdateEvent& ev)
{
    //if tank is full goto full state
    if( ev.tank_temp < _cryo_empty_temp_thresh )
    {
        _lg.strm(sl::debug) << "cryo appears to be full...";
        transit<CryostatFull>();
    }
};
