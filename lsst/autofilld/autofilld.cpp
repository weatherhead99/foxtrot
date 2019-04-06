#include <string>
#include <thread>
#include <chrono>
#include <memory>

#include <boost/program_options.hpp>

#include <foxtrot/backward.hpp>
#include <foxtrot/Logging.h>
#include <foxtrot/client/client.h>

#include "config.h"

#include "autofill_logger.hh"
#include "autofill_fill_logic.hh"

namespace po = boost::program_options;
using std::string;

int main ( int argc, char** argv )
{
    backward::SignalHandling sh;
    foxtrot::Logging lg ( "autofilld" );

    auto config_file = foxtrot::get_config_file_path ( "AUTOFILLD_CONFIG", "autofilld.config" );
    foxtrot::create_config_file ( config_file );
    lg.strm ( sl::info ) << "config file:" << config_file;


    po::options_description desc ( "autofilld allowed options" );

    int debug_level;
    int log_interval_s;
    int rotate_time_m;
    string logdir;
    string connstr;
    string webconf;
    string basefname;
    string pb_channel;

    double limit_temp;
    double limit_pres;
    bool dryrun;

    desc.add_options()
    ( "logdir,l", po::value<string> ( &logdir )->required(), "location to log events" )
    ( "interval,i", po::value<int> ( &log_interval_s )->default_value ( 10 ),
      "seconds between logging points" )
    ( "debug,d", po::value<int> ( &debug_level )->default_value ( 4 ), "debug level" )
    ( "basefname",po::value<string> ( &basefname )->default_value ( "log" ),"base name for log files" )
    ( "rotate_m,r", po::value<int> ( &rotate_time_m )->default_value ( 24 * 60 ),
      "minutes before rotating a log file" )
    ( "connstr,c",po::value<string> ( &connstr )->default_value ( "localhost:50051" ), "connection string for foxtrot client" )
    ( "limit_temp,T",po::value<double> ( &limit_temp )->default_value ( -195.0 ),
      "temperature above which to fill tank" )
    ( "limit_pressure,P",po::value<double> ( &limit_pres )->default_value ( 1E-4 ),
      "pressure above which tank will not be filled" )
    ("dryrun,D", po::bool_switch(&dryrun),"dryrun - do not actually do fills")
    ("pb_channel", po::value<string>(&pb_channel), "pushbullet channel for notifications")
    ( "help","produce help message" );

    po::variables_map vm;
    po::store ( po::command_line_parser ( argc,argv ).options ( desc ).run(), vm );
    foxtrot::load_config_file ( config_file,desc,vm,&lg );

    if ( vm.count ( "help" ) ) {
        std::cout << desc << std::endl;
        std::exit ( 0 );
    }

    try {
        po::notify ( vm );
    } catch ( boost::program_options::required_option& err ) {
        lg.strm ( sl::fatal ) << "required option missing...";
        lg.strm ( sl::fatal ) << err.what();
        std::exit ( 1 );
    }

    foxtrot::check_debug_level_and_exit ( debug_level, lg );
    foxtrot::setDefaultSink();

    lg.strm ( sl::info ) << "connecting client..." ;
    foxtrot::Client cl ( connstr );

    foxtrot::autofill_logger logger ( logdir, rotate_time_m );

    
    std::unique_ptr<foxtrot::autofill_logic> logicptr{};
    if(vm.count("pb_channel"))
        logicptr.reset(new foxtrot::autofill_logic(logger,limit_pres,limit_temp,dryrun,
                                                   &pb_channel));
    else
        logicptr.reset(new foxtrot::autofill_logic(logger,limit_pres,limit_temp,dryrun));


    auto filerotate = logger.rotate_files_async_start ( basefname );

    logicptr->register_devid(cl);

    while ( true ) {
    lg.strm ( sl::debug ) << "waiting for tick time" ;
        std::this_thread::sleep_for ( std::chrono::seconds ( log_interval_s ) );
        lg.strm ( sl::debug ) << "measuring environmental data";

        auto env_data = logicptr->measure_data ( cl );
        logger.LogEnvData(env_data);
        lg.strm ( sl::debug ) << "running autofilld tick" ;
        logicptr->tick( cl,env_data );

    };

};
