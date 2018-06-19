#pragma once
#include <boost/optional.hpp>
#include <string>
#include <cstdlib>

using std::string;

namespace foxtrot
{
    struct exptserve_options
    {
        boost::optional<string> setupfile;
        boost::optional<string> parameterfile;
        boost::optional<int> threads;
        boost::optional<int> debuglevel;
        boost::optional<string> keyfile;
        boost::optional<string> certfile;
    };
    
    
    std::string get_config_file_path();
    void create_config_file(const string& filename);
    exptserve_options load_config_from_file(const string& filename);
    
}
