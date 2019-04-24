#include <chrono>
#include <thread>
#include <boost/date_time.hpp>

#include <foxtrot/Logging.h>

#include "autofill_logger.hh"


using namespace foxtrot;
namespace pt = boost::posix_time;
namespace gg = boost::gregorian;

autofill_logger::autofill_logger(const string& outfdir, int rotate_time_mins)
: _lg("autofill_logger"), outfdir_(outfdir), rotate_time_mins_(rotate_time_mins)
{
    if(!boost::filesystem::exists(outfdir))
    {
        _lg.Info("directory doesn't exist, creating...");
        boost::filesystem::create_directories(outfdir);
        
    }
    
};


autofill_logger::~autofill_logger()
{
    if(_thisfile)
        delete _thisfile;
}

void foxtrot::autofill_logger::start_new_logfile(const std::string& name)
{
    std::lock_guard<std::mutex> lck(file_swap_m);
    
    auto outfpath = boost::filesystem::path(outfdir_);
    
    auto newfpath = outfpath / (name + ".csv");
    
    int append = 1;
    while(boost::filesystem::exists(newfpath))
    {
        _lg.strm(sl::info) << "file already exists, trying appended number: " << append;
        newfpath = outfpath / (name + "_" + std::to_string(append++) + ".csv");
    }
    
    if(_thisfile)
        delete _thisfile;
    
    
    _thisfile = new boost::filesystem::ofstream(newfpath);
    
    if(!_thisfile)
    {
        _lg.strm(sl::error) << "couldn't create new log file!";
        throw std::runtime_error("couldn't create new log file!");
    
    }
    
    *(_thisfile) << "#unixtime,datetime,pressure_cryo(hPa),pressure_pump(hPa),temp_stage(C),temp_tank(C),htr(V),target(C),cryo_gauge_onoff,pump_gauge_onoff" << std::endl;
    
}

void foxtrot::autofill_logger::LogEnvData(const foxtrot::env_data& dat)
{
    std::lock_guard<std::mutex> lck(file_swap_m);
    
    if(!_thisfile)
    {
        _lg.strm(sl::error) << "can't log without open file!";
        throw std::logic_error("can't log without open file!");
    }
    
    auto unix_epoch = (dat.timestamp - pt::from_time_t(0)).total_seconds();
    
    *(_thisfile) << unix_epoch << "," 
    << pt::to_iso_string(dat.timestamp) << 
        "," << dat.cryostat_pressure << 
        "," << dat.pump_pressure << 
        "," << dat.stage_temp << 
        "," << dat.tank_temp << 
        "," << dat.heater_output << 
        "," << dat.heater_target << 
        "," << (int) dat.pump_gauge_enable << 
        "," << (int) dat.cryo_gauge_enable << std::endl;
}

void foxtrot::autofill_logger::LogEvent(const foxtrot::event_data& evdat)
{
    std::lock_guard<std::mutex> lck(file_swap_m);
    if(!_thisfile)
    {
        _lg.strm(sl::error) << "can't log without open file!";
        throw std::logic_error("can't log without open file!");
    }
    
    auto unix_epoch = (evdat.timestamp - pt::from_time_t(0)).total_seconds();
    
    std::string evname;
    
    switch(evdat.evtp)
    {
        case(event_types::fill_begin): evname = "fill_begin"; break;
        case(event_types::fill_complete): evname = "fill_complete"; break;
        case(event_types::tank_empty): evname = "tank_empty"; break;
        case(event_types::dewar_empty): evname = "dewar_empty"; break;
        case(event_types::dewar_filled): evname="dewar_filled"; break;
        default: throw std::logic_error("invalid event");
    }
        
    *(_thisfile) << "#EVENT:" << unix_epoch << "," << pt::to_iso_string(evdat.timestamp) << "," << evname << std::endl;
        
    
}

void foxtrot::autofill_logger::LogComment(const string& comment)
{
    std::lock_guard<std::mutex> lck(file_swap_m);
    if(!_thisfile)
    {
        _lg.strm(sl::error) << "can't log without open file!";
        throw std::logic_error("can't log without open file!");
    }
    
    auto now = pt::second_clock::local_time();
    auto unix_epoch = (now - pt::from_time_t(0)).total_seconds();
    
    *(_thisfile) << "#" <<  unix_epoch << "," << pt::to_iso_string(now) <<"," << comment << std::endl;
    
    
}



string foxtrot::autofill_logger::get_date_time()
{
    auto now = pt::second_clock::local_time();
    auto timeofday = now.time_of_day();
    std::ostringstream oss;
    oss << gg::to_iso_extended_string(now.date()) << "T" 
    << std::setw(2) << std::setfill('0') << timeofday.hours() << "-" 
    << std::setw(2) << std::setfill('0') << timeofday.minutes();
    return oss.str();
}


std::future<std::exception_ptr> foxtrot::autofill_logger::rotate_files_async_start(const string& basefname)
{
    auto launchfun = [this, &basefname] () {
        std::exception_ptr except;
        foxtrot::Logging lg("rotate_files_async_start");
        try {
            while(true)
            {
                lg.strm(sl::debug) << "starting new file automatically...";
                auto fnamedt = this->get_date_time();
                this->start_new_logfile(basefname + std::string{"_"} + fnamedt);
                lg.strm(sl::debug) << "waiting for: " << rotate_time_mins_ << " minutes";
                std::this_thread::sleep_for(std::chrono::minutes(rotate_time_mins_));
                
            }
        }
        catch(...)
        {
            except = std::current_exception();
        }
        return except;
    };
    
    return std::async(std::launch::async,launchfun);
}
