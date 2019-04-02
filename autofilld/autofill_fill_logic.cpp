#include "autofill_fill_logic.hh"
#include "client.h"
#include <thread>
#include <chrono>
#include "autofill_common.hh"
#include <boost/date_time.hpp>
#include "autofill_logger.hh"

namespace pt = boost::posix_time;

using namespace foxtrot;

autofill_logic::autofill_logic(autofill_logger& logger, double limit_pressure,
    double empty_temp)
: logger_(logger), lg_("autofill_logic"), empty_temp_(empty_temp), limit_pressure_(limit_pressure)
{
    fill_in_progress = false;
    fill_just_done = false;
    
}


void autofill_logic::register_devid(Client& cl)
{
    auto sd = cl.DescribeServer();
    auto devid = foxtrot::find_devid_on_server(sd,"webswitch_plus");
    if(devid == -1)
        throw std::runtime_error("can't find webswitch plus on server!");
    ws_devid = devid;
    
    devid = foxtrot::find_devid_on_server(sd,"TPG362");
    if(devid == -1)
        throw std::runtime_error("can't find TPG362 on server!");
    tpg_devid = devid;
    
    devid = foxtrot::find_devid_on_server(sd,"ArchonHeaterX");
    if(devid == -1)
        throw std::runtime_error("can't find ArchonHeaterX on server");
    
    heater_devid = devid;
    

};

env_data autofill_logic::measure_data(Client& cl)
{
    env_data out;
    
    using boost::get;
    
    out.cryostat_pressure = get<double>(cl.InvokeCapability(tpg_devid,"getPressure",1));
    out.pump_pressure = get<double>(cl.InvokeCapability(tpg_devid,"getPressure",2));
    out.tank_temp = get<double>(cl.InvokeCapability(heater_devid, "getTempA"));
    out.stage_temp = get<double>(cl.InvokeCapability(heater_devid, "getTempB"));
    
    out.heater_output = get<double>(cl.InvokeCapability(heater_devid,"getHeaterAOutput"));
    
    out.heater_target = get<double>(cl.InvokeCapability(heater_devid,"getHeaterTarget",0));
    
    return out;
    
};


void autofill_logic::fill_tank(Client& cl, int ws_devid, double filltime_hours, int relay)
{
    lg_.strm(sl::trace) << "logging fill start";
    event_data evdat_start{pt::second_clock::local_time(), event_types::fill_begin};
    logger_.LogEvent(evdat_start);
    
    lg_.strm(sl::trace) << "setting fill in progress flag";
    cl.set_server_flag("fillactive", true);
    
    auto fill_dispatch = [this,&cl, ws_devid, filltime_hours, relay] () {
        fill_in_progress = true;
        detail::execute_fill(cl,ws_devid,filltime_hours,relay);
        event_data evdat_stop{pt::second_clock::local_time(), event_types::fill_complete};
        logger_.LogEvent(evdat_stop);
        cl.set_server_flag("fillactive", false);
        fill_in_progress = false;
        fill_just_done = true;
    };
    
    lg_.strm(sl::debug) << "launching async fill thread";
    std::async(std::launch::async, fill_dispatch);
    
    
}

void autofill_logic::tick(Client& cl, const env_data& env)
{
    lg_.strm(sl::trace) << "checking vacuum status..." ;
    bool pumpdown = false;
    if(env.cryostat_pressure < limit_pressure_)
    {
        lg_.strm(sl::trace) << "vacuum is pumped down";
        set_pumpdown_status(cl, true);
        pumpdown =true;
    }
    else
    {
        lg_.strm(sl::trace) << "vacuum is not pumped down";
        set_pumpdown_status(cl,false);
    }
 
    lg_.strm(sl::trace) << "checking tank temperature...";
    bool fill_needed = false;
    
    if(was_dewar_filled(cl))
    {
        set_dewar_status(cl,true);
        dewar_empty = false;
        clear_dewar_filled(cl);
        auto now = pt::second_clock::local_time();
        logger_.LogEvent(event_data{now,event_types::dewar_filled});
        
    }
    
    if(env.tank_temp > empty_temp_)
    {
        lg_.strm(sl::trace) << "cryostat tank is empty";
        auto now = pt::second_clock::local_time();
        logger_.LogEvent(event_data{now,event_types::tank_empty});
        
        set_tank_status(cl, false);
        if(fill_just_done)
        {
            lg_.strm(sl::info) << "dewar seems to be empty";
            set_dewar_status(cl, false);
            auto now = pt::second_clock::local_time();
            logger_.LogEvent(event_data{now,event_types::dewar_empty});
            dewar_empty = true;
        }
        else if(!fill_in_progress)
            fill_needed = true;
        
    }
    else
    {
        lg_.strm(sl::trace) << "cryostat tank is full";
        set_tank_status(cl,true);
    }
    
    checkin(cl);
    if(fill_needed && pumpdown && !fill_just_done)
    {
        lg_.strm(sl::info) << "logic thinks fill is needed";
        fill_tank(cl, ws_devid, 0.4, 1);    
    }
    
    fill_just_done = false;
};

bool autofill_logic::was_dewar_filled(Client& cl)
{
    try {
        auto ret = cl.get_server_flag("dewar_filled");
        return boost::get<bool>(ret);
    }
    catch(std::out_of_range& err)
    {
        lg_.strm(sl::info) << "dewar fill flag doesn't exist, creating with value false";
        cl.set_server_flag("dewar_filled",false);
        return false;
    }
    
    
}


void autofill_logic::clear_dewar_filled(Client& cl)
{
    cl.set_server_flag("dewar_filled",false);
};

bool autofill_logic::is_autofill_enabled(Client& cl)
{
    try{
        auto ret = cl.get_server_flag("autofill_enable");
        return boost::get<bool>(ret);
    }
    catch(std::out_of_range& err)
    {
        lg_.strm(sl::info) << "server flag doesn't exist, creating with value false";
        cl.set_server_flag("autofill_enable", false);
        return false;
    }
    
};

void autofill_logic::checkin(Client& cl)
{
    lg_.strm(sl::trace) << "setting last update time";
    auto now = pt::second_clock::local_time();
    cl.set_server_flag("autofill_checkin",pt::to_iso_string(now));
    
};

void autofill_logic::set_tank_status(Client& cl, bool full)
{
    lg_.strm(sl::trace) << "setting tank status flag";
    cl.set_server_flag("tank_full",full);
    
};

void autofill_logic::set_pumpdown_status(Client& cl, bool pumpeddown)
{
    lg_.strm(sl::trace) << "setting pumpdown status flag";
    cl.set_server_flag("pumpdown",pumpeddown);
};

void autofill_logic::set_dewar_status(Client& cl, bool full)
{
    lg_.strm(sl::trace) << "setting dewar status flag";
    cl.set_server_flag("dewarfull",full);
};


void detail::execute_fill(Client& cl, int ws_devid, double filltime_hours,
                          int relay)
{

    Logging lg("execute_fill");
    
    lg.strm(sl::debug) << "energising relay..." ;
    
    std::vector<foxtrot::ft_variant> args {relay, true};
    cl.InvokeCapability(ws_devid,"SetRelay", args.begin(), args.end());
    lg.strm(sl::debug) << "sleeping for fill time: " << filltime_hours << "hours" ;
    auto seconds = std::chrono::seconds( (int) filltime_hours * 3600);
    std::this_thread::sleep_for(seconds);
    
    lg.strm(sl::debug) << "turning off relay...";
    args[1] = false;
    cl.InvokeCapability(ws_devid,"SetRelay", args.begin(), args.end());
    
    
}
