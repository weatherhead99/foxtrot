#include "autofill_fill_logic.hh"
#include "client.h"
#include <thread>
#include <chrono>
#include "autofill_common.hh"
#include <boost/date_time.hpp>
#include "autofill_logger.hh"

namespace pt = boost::posix_time;

using namespace foxtrot;

autofill_logic::autofill_logic(autofill_logger& logger)
: logger_(logger), lg_("autofill_logic")
{
    
}

void autofill_logic::fill_tank(Client& cl, int ws_devid, double filltime_hours, int relay)
{
    lg_.strm(sl::trace) << "logging fill start";
    event_data evdat_start{pt::second_clock::local_time(), event_types::fill_begin};
    logger_.LogEvent(evdat_start);
    
    auto fill_dispatch = [this,&cl, ws_devid, filltime_hours, relay] () {
        fill_in_progress = true;
        detail::execute_fill(cl,ws_devid,filltime_hours,relay);
        event_data evdat_stop{pt::second_clock::local_time(), event_types::fill_complete};
        logger_.LogEvent(evdat_stop);
        
    };
    
    lg_.strm(sl::debug) << "launching async fill thread";
    std::async(std::launch::async, fill_dispatch);
    
    
}

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
