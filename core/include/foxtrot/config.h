#pragma once
#include <string>
#include <cstdlib>
#include <optional>

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
      std::optional<string> setupfile;
        std::optional<string> parameterfile;
        std::optional<int> threads;
        std::optional<int> debuglevel;
        std::optional<string> keyfile;
        std::optional<string> certfile;
    };
    
    
    std::string get_config_file_path(const std::string& envvarname = "FOXTROT_CONFIG", 
                                    const std::string& defaultfilename="exptserve.config");
    
    
    void create_config_file(const string& filename);
    
    bool load_config_file(const std::string& path,
                          boost::program_options::options_description& desc,
                          boost::program_options::variables_map& vm,
                          foxtrot::Logging* logger = nullptr);
    
    
    
    void check_debug_level_and_exit(int lvl, foxtrot::Logging& lg);
    
}
