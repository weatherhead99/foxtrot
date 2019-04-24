#include <thread>
#include <chrono>

#include <boost/date_time.hpp>

#include <foxtrot/client/client.h>
#include <foxtrot/ServerError.h>

#include "autofill_fill_logic.hh"
#include "autofill_common.hh"
#include "autofill_logger.hh"

namespace pt = boost::posix_time;

using namespace foxtrot;

autofill_logic::autofill_logic(autofill_logger& logger, double limit_pressure,
    double empty_temp, bool dryrun, const string* pb_channel)
: logger_(logger), lg_("autofill_logic"), empty_temp_(empty_temp), 
limit_pressure_(limit_pressure), dryrun_(dryrun)
{
    fill_in_progress = false;
    fill_just_done = false;

    if(pb_channel)
    {
        pb_enable = true;
        pb_channel_ = *pb_channel;
    }

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
    
    devid = foxtrot::find_devid_on_server(sd,"archon");
    if(devid == -1)
        throw std::runtime_error("can't find archon on server");
    archon_devid = devid;

};

env_data autofill_logic::measure_data(Client& cl)
{
    env_data out;

    using boost::get;
    
    cl.InvokeCapability(archon_devid,"update_state");
    
    out.timestamp = pt::second_clock::local_time();

    out.cryostat_pressure = get<double>(cl.InvokeCapability(tpg_devid,"getPressure",1));
    out.pump_pressure = get<double>(cl.InvokeCapability(tpg_devid,"getPressure",2));
    out.tank_temp = get<double>(cl.InvokeCapability(heater_devid, "getTempA"));
    out.stage_temp = get<double>(cl.InvokeCapability(heater_devid, "getTempB"));

    out.heater_output = get<double>(cl.InvokeCapability(heater_devid,"getHeaterAOutput"));

    out.heater_target = get<double>(cl.InvokeCapability(heater_devid,"getHeaterTarget",0));

    
    out.cryo_gauge_enable = get<bool>(cl.InvokeCapability(tpg_devid,"getGaugeOnOff",1));
    out.pump_gauge_enable = get<bool>(cl.InvokeCapability(tpg_devid,"getGaugeOnOff",2));
    
    return out;

};


std::future<std::exception_ptr> autofill_logic::fill_tank(Client& cl, int ws_devid, double filltime_hours, int relay)
{
    lg_.strm(sl::trace) << "logging fill start";
    event_data evdat_start{pt::second_clock::local_time(), event_types::fill_begin};
    logger_.LogEvent(evdat_start);
    
    lg_.strm(sl::trace) << "setting fill in progress flag";
    cl.set_server_flag("autofill/fillactive", true);
    
    auto fill_dispatch = [this,&cl, ws_devid, filltime_hours, relay] () {
        std::exception_ptr except;
        fill_in_progress = true;
        try{
            detail::execute_fill(cl,ws_devid,filltime_hours,relay,dryrun_);
        }
        catch(...)
        {
            except = std::current_exception();
        };
        event_data evdat_stop{pt::second_clock::local_time(), event_types::fill_complete};
        logger_.LogEvent(evdat_stop);
        cl.set_server_flag("autofill/fillactive", false);
        fill_in_progress = false;
        fill_just_done = true;
        return except;
    };
    
    lg_.strm(sl::debug) << "launching async fill thread";
    auto fut = std::async(std::launch::async, fill_dispatch);
    lg_.strm(sl::debug) << "fill thread launched";
    return fut;
    
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
        broadcast_notify(cl,"dewar","LN2 Dewar was filled");
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
        if(!fill_in_progress && !logged_tank_empty)
        {
            broadcast_notify(cl,"tank", "cryostat tank is empty");
            logger_.LogEvent(event_data{now,event_types::tank_empty});
            logged_tank_empty = true;
        }
        
        set_tank_status(cl, false);
        if(fill_just_done)
        {
            logged_tank_empty = false;
            lg_.strm(sl::info) << "dewar seems to be empty";
            
            //NOTE: only broadcast if we didn't know dewar was empty
            if(!dewar_empty)
            {
                broadcast_notify(cl,"dewar", "LN2 dewar is probably empty");
                set_dewar_status(cl, false);
                auto now = pt::second_clock::local_time();
                logger_.LogEvent(event_data{now,event_types::dewar_empty});
            }
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
    if(fill_needed && pumpdown && !fill_just_done && !dewar_empty)
    {
        lg_.strm(sl::info) << "logic thinks fill is needed";
        if(is_autofill_enabled(cl))
        {
            broadcast_notify(cl,"tank", "a tank autofill is starting");
            running_fill_ = fill_tank(cl, ws_devid, 0.4, 1);    
        }
        else
            lg_.strm(sl::info) << "autofill is disabled, not doing fill";
    }
    else if(dewar_empty)
    {
        lg_.strm(sl::info) << "not doing fill because dewar seems to be empty";
    }
    
    fill_just_done = false;
    
    
    if(running_fill_.valid() &&
        running_fill_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        auto eptr = running_fill_.get();
        if(eptr)
        {
            lg_.strm(sl::error) << "exception ptr in future!";
            broadcast_notify(cl,"tank","there was an error filling the tank");
            std::rethrow_exception(running_fill_.get());
        }
    }
    
    
};

bool autofill_logic::was_dewar_filled(Client& cl)
{
    try {
        auto ret = cl.get_server_flag("autofill/dewar_filled");
        return boost::get<bool>(ret);
    }
    catch(std::out_of_range& err)
    {
        lg_.strm(sl::info) << "dewar fill flag doesn't exist, creating with value false";
        cl.set_server_flag("autofill/dewar_filled",false);
        return false;
    }
    
    
}


void autofill_logic::clear_dewar_filled(Client& cl)
{
    cl.set_server_flag("autofill/dewar_filled",false);
};

bool autofill_logic::is_autofill_enabled(Client& cl)
{
    try{
        auto ret = cl.get_server_flag("autofill/autofill_enable");
        return boost::get<bool>(ret);
    }
    catch(std::out_of_range& err)
    {
        lg_.strm(sl::info) << "server flag doesn't exist, creating with value false";
        cl.set_server_flag("autofill/autofill_enable", false);
        return false;
    }
    
};

void autofill_logic::checkin(Client& cl)
{
    lg_.strm(sl::trace) << "setting last update time";
    auto now = pt::second_clock::local_time();
    cl.set_server_flag("autofill/autofill_checkin",pt::to_iso_string(now));
    
};

void autofill_logic::set_tank_status(Client& cl, bool full)
{
    lg_.strm(sl::trace) << "setting tank status flag";
    cl.set_server_flag("autofill/tank_full",full);
    
};

void autofill_logic::set_pumpdown_status(Client& cl, bool pumpeddown)
{
    lg_.strm(sl::trace) << "setting pumpdown status flag";
    cl.set_server_flag("autofill/pumpdown",pumpeddown);
};

void autofill_logic::set_dewar_status(Client& cl, bool full)
{
    lg_.strm(sl::trace) << "setting dewar status flag";
    cl.set_server_flag("autofill/dewarfull",full);
};


void detail::execute_fill(Client& cl, int ws_devid, double filltime_hours,
                          int relay, bool dryrun)
{

    Logging lg("execute_fill");

    lg.strm(sl::debug) << "energising relay..." ;

    std::vector<foxtrot::ft_variant> args {relay, true};
    if(!dryrun)
        cl.InvokeCapability(ws_devid,"SetRelay", args.begin(), args.end());
    else
        lg.strm(sl::debug) << "dryrun mode not energising relay";
    lg.strm(sl::debug) << "sleeping for fill time: " << filltime_hours << "hours" ;
    auto seconds = ( (int) (filltime_hours * 3600));
    lg.strm(sl::trace) << "seconds:" << seconds;
    std::this_thread::sleep_for(std::chrono::seconds(seconds));

    lg.strm(sl::debug) << "turning off relay...";
    args[1] = false;
    if(!dryrun)
        cl.InvokeCapability(ws_devid,"SetRelay", args.begin(), args.end());

}

void foxtrot::autofill_logic::broadcast_notify(foxtrot::Client& cl, const string& title,
    const string& body)
{
    if(pb_enable)
    {
        try{
            cl.BroadcastNotification(body,&title,&pb_channel_);
        }
        catch(foxtrot::ServerError& err)
        {
            lg_.strm(sl::error) << "broadcast notifications appear not to be enabled on the server";
        }
        catch(foxtrot::ProtocolError& err)
	{
	  lg_.strm(sl::error) << "broadcast protocol failed for some reason, ignoring...";
	}

    }

}



