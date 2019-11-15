#pragma once
#include <tinyfsm.hpp>
#include <foxtrot/Logging.h>
#include <foxtrot/fsmd/common_events.hh>


namespace foxtrot {
    namespace fsmd{ 



class CryoSystemFSM : public tinyfsm::Fsm<CryoSystemFSM>
{
public:
    CryoSystemFSM(double cryo_empty_temp_thresh, 
                  double allowed_pressure_thresh,
                  int fill_length_ticks
    );
    
    CryoSystemFSM();
    
    CryoSystemFSM(const CryoSystemFSM& other) = delete;
    
    void react(const tinyfsm::Event& ev);
    virtual void react(const DataUpdateEvent& ev) {};
    virtual void react(const TimeTickEvent& ev) {};
    
    virtual void entry() {};
    virtual void exit() {};
protected:
    foxtrot::Logging _lg{"CryoSystemFSM"};
    static double _cryo_empty_temp_thresh;
    static double _allowed_pressure_thresh;
    static int _fill_length_ticks;
    static bool _instance_created;
};


class IntialCryoState: public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
};

class CryostatEmpty : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
};

class CryostatFull : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
    void react(const TimeTickEvent& ev) override;
};

class CryostatFilling : public CryoSystemFSM
{
    void react(const TimeTickEvent& ev) override;
};


class TankEmpty : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
};

class TankRefilled : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
};


}
}
