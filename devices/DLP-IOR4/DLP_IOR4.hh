#pragma once
#include <foxtrot/server/CmdDevice.h>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>


using std::shared_ptr;
using foxtrot::protocols::SerialPort;

namespace foxtrot
{
    
namespace devices
{
    enum class DLP_init_relay_state: unsigned char
    {
        on,
        off,
        do_not_change
    };

    namespace detail
    {
        const std::array<DLP_init_relay_state,4> DLP_all_off_state
        {DLP_init_relay_state::off, DLP_init_relay_state::off, 
        DLP_init_relay_state::off, DLP_init_relay_state::off};
    }

    
    class DLP_IOR4  : public CmdDevice
    {
        RTTR_ENABLE()
    public:
        DLP_IOR4(shared_ptr<SerialPort> proto, 
                 const std::string& comment,
                 const std::array<DLP_init_relay_state,4>& states = detail::DLP_all_off_state );
        
        void setRelay(int n, bool onoff);
        bool getRelay(int n) const;
        const std::string getDeviceTypeName() const override;
        
    private:
        shared_ptr<SerialPort> _serport;
        std::array<DLP_init_relay_state,4> _relaystatus;
        foxtrot::Logging _lg;
    };
       
}
}
