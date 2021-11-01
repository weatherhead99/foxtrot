#pragma once
#include <tinyfsm.hpp>
#include <foxtrot/Logging.h>
#include <foxtrot/fsmd/common_events.hh>
#include <string>

using std::string;

namespace foxtrot {
    class Client;
    
    namespace fsmd{ 

class CryoSystemFSM : public tinyfsm::Fsm<CryoSystemFSM>
{
public:
    CryoSystemFSM(double cryo_empty_temp_thresh, 
                  double allowed_pressure_thresh,
                  duration_s fill_time_preset);
    
    //TODO: sort out copy constructor etc
    CryoSystemFSM() {};
    CryoSystemFSM(const CryoSystemFSM& other) = default;
    virtual std::string name() const = 0;
    
    void react(const tinyfsm::Event& ev);
    virtual void react(const DataUpdateEvent& ev) {};
    virtual void react(const TimeTickEvent& ev) {};
    
    virtual void entry();
    virtual void exit() {};
protected:
    bool should_refill(const DataUpdateEvent& ev);
    
    
    foxtrot::Logging _lg{"CryoSystemFSM"};
    static double _cryo_empty_temp_thresh;
    static double _allowed_pressure_thresh;
    static duration_s _fill_time_preset;
    static bool _instance_created;
};


class InitialCryoState: public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
    std::string name() const override;
};

class CryostatEmpty : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
    std::string name() const override;
};

class CryostatFull : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
    std::string name() const override;
    
};

class CryostatFilling : public CryoSystemFSM
{
public:
    void entry() override;
    void react(const TimeTickEvent& ev) override;
    void react(const DataUpdateEvent& ev) override;
    std::string name() const override;
    
private:
    timept _start_time;
    duration_s _filling_time;
};


class TankEmpty : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
    std::string name() const override;
};

class TankRefilled : public CryoSystemFSM
{
    void react(const DataUpdateEvent& ev) override;
    std::string name() const override;
};


}
}
