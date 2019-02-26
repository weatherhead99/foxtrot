#include "autofill_logger.hh"

#include <boost/date_time.hpp>

using namespace foxtrot;
namespace pt = boost::posix_time;

autofill_logger::autofill_logger(const string& outfdir, int rotate_time_hours)
: _lg("autofill_logger"), outfdir_(outfdir)
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
    
    *(_thisfile) << "#unixtime,datetime,pressure_cryo(hPa),pressure_pump(hPa),temp_stage(C),temp_tank(C),htr(V),target(C)" << std::endl;
    
}

void foxtrot::autofill_logger::LogEnvData(const foxtrot::env_data& dat)
{
    if(!_thisfile)
    {
        _lg.strm(sl::error) << "can't log without open file!";
        throw std::logic_error("can't log without open file!");
    }
    
    auto unix_epoch = (dat.timestamp - pt::from_time_t(0)).total_seconds();
    
    *(_thisfile) << unix_epoch << "," << pt::to_iso_string(dat.timestamp) << "," << 
        dat.cryostat_pressure << "," << dat.pump_pressure << "," << dat.stage_temp << "," 
        << dat.tank_temp << "," << dat.heater_output << "," << dat.heater_target << std::endl;
    

}

void foxtrot::autofill_logger::LogEvent(const foxtrot::event_data& evdat)
{
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
    }
        
    *(_thisfile) << "#EVENT:" << unix_epoch << "," << pt::to_iso_string(evdat.timestamp) << "," << evname << std::endl;
        
    
}


