#pragma once
#include <string>
#include <chrono>

namespace foxtrot
{
    
    class Client;

namespace fsmd
{
    using fsmtime = std::chrono::time_point<std::chrono::system_clock>;
    
    void log_event(const fsmtime& tm, const std::string& comment);
    void update_fsm_state_flags(Client* cl, const std::string& fsmname, const std::string& statename, const fsmtime& tm);
    
}
};
