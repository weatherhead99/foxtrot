#include "config.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <Logging.h>

namespace pt = boost::property_tree;



std::string foxtrot::get_config_file_path()
{
    
    foxtrot::Logging lg("get_config_file_path");
    
    char* path = std::getenv("FOXTROT_CONFIG");
    
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
        home /= "exptserve.config" ;
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
