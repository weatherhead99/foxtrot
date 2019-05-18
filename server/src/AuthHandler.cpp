#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/date_time.hpp>

#include <foxtrot/server/AuthHandler.h>

using namespace foxtrot;
using std::string;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

namespace ptime = boost::posix_time;


AuthHandler::AuthHandler(const std::string& filename)
: _lg("AuthHandler")
{
    _creds = load_creds_from_file(filename);
};


std::pair<std::string, unsigned> foxtrot::AuthHandler::get_challenge_string(const std::string& userid)
{
    if(_creds.find(userid)  == _creds.end())
    {
        _lg.Info("request for a challenge from invalid userid");
        throw std::out_of_range("invalid userid");
    };
    
    if(_challenges.size() == MAX_CHALLENGES)
    {
        std::lock_guard<std::mutex> lck(challenge_mut);
        _lg.Error("maximum number of challenges already in progress, perhaps a DoS attack?");
        unsigned remove_id = _challenge_order.front();
        _challenges.erase(_challenges.find(remove_id));
        _challenge_order.pop_front();
    };

    auto bytes = detail::get_challenge_bytes();

    std::string out = detail::bin2base64(bytes);
    unsigned challengeid = randombytes_random();
    
    {
        std::lock_guard<std::mutex> lck(challenge_mut);
        _challenges.insert({challengeid, {userid, out}});
        _challenge_order.push_back(challengeid);
        
        
    }
    
    return std::make_pair(out, challengeid);
}

bool foxtrot::AuthHandler::verify_response(const std::string& userid, unsigned int challenge_id, const std::string& response, const std::string& sig, int& authlevel, std::string& sessionkey)
{
    auto credit = _creds.find(userid);
    if(credit == _creds.end())
    {
        _lg.Info("invalid userid in auth response");
        throw std::out_of_range("invalid userid");
    }
    
    //TODO: challenges should be stored in binary form!
    std::string origchallenge;
    std::string origuserid;
    auto challengeit = _challenges.find(challenge_id);
    if(challengeit == _challenges.end())
    {
        _lg.Info("invalid challenge id");
        throw std::out_of_range("invalid challenge id");
    }
    else
    {
        origchallenge = challengeit->second.second;
        origuserid = challengeit->second.first;
        if(userid != origuserid)
        {
            _lg.Info("mismatch in user id");
            throw std::runtime_error("mismatched user id!");
        }
        if(origchallenge != response)
        {
            _lg.Info("mismatched challenge string");
            throw std::runtime_error("mismatched challenge string");
        }
    }
    
    authlevel = credit->second.second;
    bool verify = false;
    std::string keyname;
    for(auto& key : credit->second.first)
    {
        bool thisverify = crypto_sign_verify_detached(
            reinterpret_cast<const unsigned char*>(sig.data()),
            reinterpret_cast<const unsigned char*>(response.data()),response.size(),key.second.data());
        if(thisverify)
        {
            std::lock_guard<std::mutex> lck(session_mut);
            _lg.strm(sl::info) << "verified login for: " << userid << "with key name: " << key.first;
            keyname = key.first;
            
            seskeyarr seskey;
            randombytes_buf(seskey.begin(),seskey.size());
            //TODO:FINISH HERE!
            
            return true;
        }
    }
    _lg.strm(sl::info) << "failed login attempt for: " << userid;
    return false;
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
        std::vector<key_info> keys;
        for(auto& key: keytree)
        {
            
            _lg.strm(sl::debug) << "loading key named: " << key.first;
            auto keystr = key.second.get_value<string>();
            auto keyvec = detail::base642bin(keystr);
            pkarr keyarr;
            std::move(keyvec.begin(), keyvec.begin() + keyvec.size(), keyarr.begin());
            
            keys.push_back(std::make_pair(key.first, keyarr));
        }
        auto authlevel = item.second.get_child("authlevel").get_value<int>();
        
        _creds.insert({userid,{keys,authlevel}});
    };
    
    _lg.strm(sl::info) << "loaded: " << _creds.size() << " user credentials";
    return _creds;
}




