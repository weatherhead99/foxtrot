#include <string>

#include <boost/program_options.hpp>
#include "client.h"
#include "config.h"
#include "Logging.h"
#include <backward.hpp>

#include "autofill_logger.hh"
#include "autofill_fill_logic.hh"
#include <thread>
#include <chrono>

namespace po = boost::program_options;
using std::string;

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    foxtrot::Logging lg("autofilld");
    
    auto config_file = foxtrot::get_config_file_path("AUTOFILLD_CONFIG", "autofilld.config");
    foxtrot::create_config_file(config_file);
    lg.strm(sl::info) << "config file:" << config_file;
    
    
    po::options_description desc("autofilld allowed options");
    
    int debug_level;
    int log_interval_s;
    int rotate_time_m;
    string logdir;
    string connstr;
    string webconf;
    
    double limit_temp;
    double limit_pres;
    
    desc.add_options()
    ("logdir,l", po::value<string>(&logdir)->required(), "location to log events")
    ("interval,i", po::value<int>(&log_interval_s)->default_value(10),
     "seconds between logging points")
    ("debug,d", po::value<int>(&debug_level)->default_value(4), "debug level")
    ("rotate_m,r", po::value<int>(&rotate_time_m)->default_value(24 * 60),
     "minutes before rotating a log file")
    ("connstr,c",po::value<string>(&connstr)->default_value("localhost:50051"), "connection string for foxtrot client")
    ("limit_temp,T",po::value<double>(&limit_temp)->default_value(-195.0),
     "temperature above which to fill tank")
    ("limit_pressure,P",po::value<double>(&limit_pres)->default_value(1E-4),
     "pressure above which tank will not be filled")
    ("help","produce help message");
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).run(), vm);
    foxtrot::load_config_file(config_file,desc,vm,&lg);
    
    if(vm.count("help"))
    {
        std::cout << desc << std::endl;
        std::exit(0);
    }
        
    try {
        po::notify(vm);
    }
    catch(boost::program_options::required_option& err)
    {
        lg.strm(sl::fatal) << "required option missing...";
        lg.strm(sl::fatal) << err.what();
        std::exit(1);
    }
    
    foxtrot::check_debug_level_and_exit(debug_level, lg);
    foxtrot::setDefaultSink();
        
    lg.strm(sl::info) << "connecting client..." ;
    foxtrot::Client cl(connstr);
    
    foxtrot::autofill_logger logger(logdir, rotate_time_m);
    
    std::cout << "starting new logfile..." << std::endl;
    
    foxtrot::autofill_logic logic(logger,limit_pres,limit_temp);
    
    
    
};
