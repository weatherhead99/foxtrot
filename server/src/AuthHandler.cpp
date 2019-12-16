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


AuthHandler::AuthHandler(const std::string& filename, int valid_hours)
: _lg("AuthHandler"), valid_hours_(valid_hours)
{
    _creds = load_creds_from_file(filename);
};


std::pair<std::string, unsigned> foxtrot::AuthHandler::get_challenge_string(const std::string& userid)
{
    auto challenge_binary = get_challenge_binary(userid);
    auto challengestr = detail::bin2base64(challenge_binary.first);
    
    return std::make_pair(challengestr, challenge_binary.second);
}


std::pair<foxtrot::challengearr, unsigned>
foxtrot::AuthHandler::get_challenge_binary(const std::string& userid)
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

    unsigned challengeid = randombytes_random();
    
    {
        std::lock_guard<std::mutex> lck(challenge_mut);
        _challenges.insert({challengeid, {userid, bytes}});
        _challenge_order.push_back(challengeid);
        
        
    }
    
    return std::make_pair(bytes, challengeid);
    
    
};

bool foxtrot::AuthHandler::verify_response(const std::string& userid, unsigned int challenge_id, const foxtrot::sigarr& sig, int& authlevel, time_t& expiry, seskeyarr& sessionkey)
{
    auto credit = _creds.find(userid);
    if(credit == _creds.end())
    {
        _lg.Info("invalid userid in auth response");
        throw std::out_of_range("invalid userid");
    }
    
    //TODO: challenges should be stored in binary form!
    std::string origuserid;
    challengearr origchallenge;
    
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
        
        std::lock_guard<std::mutex> lck(challenge_mut);
        _challenges.erase(challengeit);
        _challenge_order.pop_front();
        
        
        if(userid != origuserid)
        {
            _lg.Info("mismatch in user id");
            throw std::runtime_error("mismatched user id!");
        }
    }
    
    authlevel = credit->second.second;
    _lg.strm(sl::trace) << "user authlevel is: " << authlevel;
    bool verify = false;
    std::string keyname;
    for(auto& key : credit->second.first)
    {
        _lg.strm(sl::trace) << "trying to verify for keyname: " << key.first;
        
        std::ostringstream oss;
        for(auto& c : sig)
        {
          oss << std::to_string((int) c) << ",";  
            
        };
        _lg.strm(sl::debug) << "sig bytes: "  << oss.str(); 
        
        oss.str("");
        
        for(auto& c : origchallenge)
        {
            oss << std::to_string((int) c)  << ",";
        };
        _lg.strm(sl::debug) << "challenge bytes: " << oss.str();
        
        oss.str("");
        for(auto& c : key.second)
        {
            oss << std::to_string((int) c ) << ",";
        }
        _lg.strm(sl::debug) << "public key bytes: " << oss.str();
        
        
        int thisverify = crypto_sign_verify_detached(sig.data(),
            origchallenge.data(),origchallenge.size(),key.second.data());
        if(thisverify == 0 )
        {
            _lg.strm(sl::info) << "verified login for: " << userid << "with key name: " << key.first;
            keyname = key.first;
            
            randombytes_buf(sessionkey.data(),sessionkey.size());
            
            auto expiry_date = boost::posix_time::second_clock::local_time() + boost::posix_time::hours(valid_hours_);
            expiry = (expiry_date - boost::posix_time::from_time_t(0)).total_seconds();
            
            //find the auth level
            if(credit == _creds.end())
            {
                throw std::logic_error("invalid credential after valid login. This shouldn't happen!");
            }
            
            authlevel = credit->second.second;
            
            std::lock_guard<std::mutex> lck(session_mut);
            login_info login{userid, authlevel, expiry};
            _sessionkeys.insert({sessionkey, login});
            
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




