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
        _lg.strm(sl::error) << "file: " << filename << "doesn't seem to exist";
        throw std::out_of_range("credentials file couldn't be found!");
    }

    pt::ptree credstree;

    _lg.strm(sl::info) << "loading credentials from file: " << filename;
    pt::read_json(filename, credstree);
    
    for(auto& item: credstree)
    {
        std::string userid = item.first;
        _lg.strm(sl::debug) << "found userid: " << userid;

        auto keytree = item.second.get_child("keys");
        credentialsmap::mapped_type keys;
        for(auto& key: keytree)
        {
            
            _lg.strm(sl::debug) << "loading key named: " << key.first;
            auto keystr = key.second.get_value<string>();
            auto keyvec = detail::base642bin(keystr);
            pkarr keyarr;
            std::move(keyvec.begin(), keyvec.begin() + keyvec.size(), keyarr.begin());
            
            keys.push_back(std::make_pair(key.first, keyarr));
        }
        
        _creds.insert({userid,keys});
    };
    
    _lg.strm(sl::info) << "loaded: " << _creds.size() << " user credentials";
    return _creds;
}




