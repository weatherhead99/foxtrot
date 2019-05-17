#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem/operations.hpp>


#include <foxtrot/server/AuthHandler.h>

using namespace foxtrot;
using std::string;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

AuthHandler::AuthHandler(const std::string& filename)
: _lg("AuthHandler")
{
    _creds = load_creds_from_file(filename);
    
};


std::string foxtrot::AuthHandler::get_challenge_string() const
{
    auto bytes = detail::get_challenge_bytes();
    auto len = sodium_base64_encoded_len(bytes.size(),
                                         sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
    
    std::string out = detail::bin2base64(bytes);
    return out;
}



foxtrot::credentialsmap foxtrot::AuthHandler::load_creds_from_file(const std::string& filename)
{
    if(!fs::exists(filename))
    {
        throw std::out_of_range("credentials file couldn't be found!");
    }

    pt::ptree credstree;

    _lg.strm(sl::info) << "loading credentials from file: " << filename;
    pt::read_json(filename, credstree);
    
    for(auto& item: credstree)
    {
        std::string userid = item.first;
        _lg.strm(sl::debug) << "found userid: " << userid;

        std::vector<string> keys;
        for(auto& key: item.second)
        {
            _lg.strm(sl::debug) << "loading key named: " << key.first;
            keys.push_back(key.second.get_value<string>());
        }
        
        _creds.insert({userid,keys});
    };
    
    _lg.strm(sl::info) << "loaded: " << _creds.size() << " user credentials";

}




