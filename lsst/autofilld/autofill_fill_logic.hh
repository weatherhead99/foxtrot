#pragma once
#include <atomic>
#include <future>

#include <foxtrot/Logging.h>

using std::string;

namespace foxtrot {
    class Client;
    class autofill_logger;
    class Logging;
    struct env_data;
    
    
    class autofill_logic {
    public:
        autofill_logic(autofill_logger& logger, double limit_pressure,
            double empty_temp, bool dryrun=false, const string* pb_channel = nullptr);
        
        void register_devid(Client& cl);
        env_data measure_data(Client& cl);
        
        std::future<std::exception_ptr> fill_tank(Client& cl, int ws_devid, double filltime_hours, int relay);
        
        void tick(Client& cl, const env_data& env);
        
        bool was_dewar_filled(Client& cl);
        bool is_autofill_enabled(Client& cl);
        void set_tank_status(Client& cl, bool full);
        void set_pumpdown_status(Client& cl, bool pumpeddown);
        void set_dewar_status(Client& cl, bool full);
        void checkin(Client& cl);
        void clear_dewar_filled(Client& cl);
        void broadcast_notify(Client& cl, const string& title, const string& body);
        
    private:
        int ws_devid = -1;
        int tpg_devid = -1;
        int heater_devid = -1;
        int archon_devid = -1;
        bool dryrun_;
        bool logged_tank_empty = false;
        double empty_temp_;
        double limit_pressure_;
        autofill_logger& logger_;
        bool dewar_empty = false;
        std::atomic<bool> fill_in_progress;
        std::atomic<bool> fill_just_done;
        Logging lg_;
        std::future<std::exception_ptr> running_fill_;
        bool pb_enable = false;
        string pb_channel_ = "";
        
    };
    
    
    namespace detail
    {
        
        void execute_fill(Client& cl, int ws_devid, double filltime_hours, int relay, bool dryrun=false);
        
    
    }
}
