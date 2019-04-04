#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include <foxtrot/Logging.h>

#include "config.h"

namespace pt = boost::property_tree;
namespace po = boost::program_options;


std::string foxtrot::get_config_file_path(const std::string& envvarname, 
                                             const std::string& defaultfilename)
{
       
    foxtrot::Logging lg("get_config_file_path");
    
    char* path = std::getenv(envvarname.c_str());
    
    if(path == nullptr)
    {
        lg.Info("no environment variable, getting config from default path");
#ifdef linux
        char* homeenv = std::getenv("HOME");
#else
        char* homedrive = std::getenv("HOMEDRIVE");
        char* homepath = std::getenv("HOMEPATH");
        
        auto homeenv = boost::filesystem::path(homedrive);
        homeenv /= homepath;
        
#endif
        auto home = boost::filesystem::path(homeenv);
        home /= ".foxtrot" ;
        home /= defaultfilename ;
        return home.string();
    }
    else
    {
        return std::string(path);
    }
    
    
}


void foxtrot::create_config_file(const string& filename)
{
    foxtrot::Logging lg("create_config_file");
    if(!boost::filesystem::exists(filename))
    {
        auto path = boost::filesystem::path(filename);
        boost::filesystem::create_directories(path.parent_path());
        lg.Info("creating config file...");
        boost::filesystem::ofstream ofs(filename);
        
        if(!ofs)
        {
            lg.Error("couldn't create config file!");
            throw std::runtime_error("couldn't create config file!");
        };
        
    };
};



bool foxtrot::load_config_file(const std::string& path,
                          boost::program_options::options_description& desc,
                          boost::program_options::variables_map& vm,
                          foxtrot::Logging* logger)
{
    std::ifstream ifs(path);
    if(ifs.good())
    {
        if(logger)
            logger->strm(sl::debug) << "successfully opened config file, parsing...";
        
    
            po::store(po::parse_config_file(ifs,desc),vm);
        if(logger)
            logger->strm(sl::trace) << "stored parsed variables from config file";
        
        return true;
    }
    else
    {
        if(logger)
            logger->strm(sl::debug) << "can't open specified config file for reading...";
        return false;
    }
    
}
    
void foxtrot::check_debug_level_and_exit(int lvl, foxtrot::Logging& lg)
{
    if(lvl < 0 || lvl > 5)
    {
        lg.Fatal("invalid debug level specified!");
        std::exit(1);
    }
    
    foxtrot::setLogFilterLevel(static_cast<sl>(5 - lvl));
    
}

