#pragma once
#include <boost/optional.hpp>
#include <string>
#include <cstdlib>

using std::string;

namespace boost{
    namespace program_options {
        class variables_map;
        class options_description;
    }
}

namespace foxtrot
{
    class Logging; 
    
    struct exptserve_options
    {
        boost::optional<string> setupfile;
        boost::optional<string> parameterfile;
        boost::optional<int> threads;
        boost::optional<int> debuglevel;
        boost::optional<string> keyfile;
        boost::optional<string> certfile;
    };
    
    
    std::string get_config_file_path(const std::string& envvarname = "FOXTROT_CONFIG", 
                                    const std::string& defaultfilename="exptserve.config");
    
    
    void create_config_file(const string& filename);
    exptserve_options load_config_from_file(const string& filename);
    
    void load_config_file(const std::string& path,
                          boost::program_options::options_description& desc,
                          boost::program_options::variables_map& vm,
                          foxtrot::Logging* logger = nullptr);
    
    
    
}
