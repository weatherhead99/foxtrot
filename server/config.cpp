#include "config.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <Logging.h>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>


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


foxtrot::exptserve_options foxtrot::load_config_from_file(const string& filename)
{
    exptserve_options out;
    boost::filesystem::path path(filename);
    
    if(!boost::filesystem::exists(path))
    {
        throw std::runtime_error("couldn't open config file...");
    };
    
    pt::ptree tree;
    pt::read_ini(filename,tree);
    
    out.setupfile = tree.get_optional<string>("exptserve.setupfile");
    out.parameterfile = tree.get_optional<string>("exptserve.parameterfile");
    out.threads = tree.get_optional<int>("exptserve.threads");
    out.debuglevel = tree.get_optional<int>("exptserve.debuglevel");
    out.keyfile = tree.get_optional<string>("exptserve.keyfile");
    out.certfile = tree.get_optional<string>("exptserve.certfile");
    
    return out;
}

bool load_config_file(const std::string& path,
                          boost::program_options::options_description& desc,
                          boost::program_options::variables_map& vm,
                          foxtrot::Logging* logger = nullptr)
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
    


