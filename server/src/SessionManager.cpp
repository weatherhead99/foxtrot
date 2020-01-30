#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/server/auth_utils.h>
#include <foxtrot/ContentionError.h>
#include <sodium.h>
#include <algorithm>
#include <iomanip>
#include <thread>

using namespace foxtrot;

auto put_time_helper(const time_type& time, const string& format="%c")
{
    auto tt = std::chrono::system_clock::to_time_t(time);
    auto localtime = std::localtime(&tt);
    return std::put_time(localtime, format.c_str());
}


foxtrot::SessionManager::SessionManager(const duration_type& session_length)
: _lg("SessionManager"), _session_length(session_length)
{
}



const foxtrot::session_info & foxtrot::SessionManager::get_session_info(const foxtrot::Sessionid& session_id) const
{
    
}


const foxtrot::session_info *const foxtrot::SessionManager::who_owns_device(unsigned short devid) const
{
    return find_in_cache(_devicecachemap, devid);
}

const foxtrot::session_info *const foxtrot::SessionManager::who_owns_flag(const std::string& flagname) const
{
    return find_in_cache(_flagcachemap, flagname);
}


foxtrot::Sessionid  foxtrot::SessionManager::start_session(
    const std::string& username, 
    const std::string& comment, 
    const vector<unsigned short> *const devices_requested, 
    const vector<std::string> *const flags_requested)
{
    std::lock_guard<std::mutex> lck(_sessionmut);
    _lg.strm(sl::info) << "new user session requested by: "<< username 
                        << "with comment: " << comment;
    
                        
    //check that devices and flags aren't already in a session
    try{
        _lg.strm(sl::debug) << "checking for requested devices..." ;
        check_requested_items(devices_requested, [this] (auto devid) { return who_owns_device(devid);});
        _lg.strm(sl::debug) << "checking for requested flags...";
        check_requested_items(flags_requested, [this] (auto flagname) { return who_owns_flag(flagname);});
    }
    catch(int i)
    {
        _lg.strm(sl::error) << "a requested item is owned by session with id: " << i;
        throw i;
    };
    
    //setup the next session internal id to be issued
    using Mt = typename decltype(_sessionmap)::value_type;
    
    if(not _sessionmap.empty())
    {
        auto it = std::max_element(_sessionmap.cbegin(), _sessionmap.cend(), 
                                    [] (const Mt& first, const Mt& second) 
                                    {
                                        return first.first < second.first;
                                    });
        next_session_id = it->first + 1;
    }
    
    session_info newsession;
    newsession.comment = comment;
    newsession.user_identifier = username;
    newsession.internal_session_id = next_session_id;
    auto now = std::chrono::system_clock::now();
    newsession.expiry = now + _session_length;
    
    if(devices_requested)
    {
        newsession.devices.insert(newsession.devices.begin(), 
                                  devices_requested->cbegin(), devices_requested->cend());
        for(auto& devid : *devices_requested)
            _devicecachemap[devid] = next_session_id;
    }
        
    if(flags_requested)
    {
        newsession.flags.insert(newsession.flags.begin(),
                                flags_requested->cbegin(), flags_requested->cend());
        for(auto& flagname : *flags_requested)
            _flagcachemap[flagname] = next_session_id;
    }
    
    _lg.strm(sl::debug) << "generating session secret...";
    auto secret = generate_session_id();
    _sessionidmap[secret] = next_session_id;
    _sessionmap[next_session_id] = newsession;
    
    
    _lg.strm(sl::info) << "session creation succeeded, expiry time is: " << put_time_helper(newsession.expiry);
    //NOTE: is this cheaper than returning the whole Sessionid object?
    return secret;
}


bool foxtrot::SessionManager::close_session(const foxtrot::Sessionid& session_id)
{
    auto loc = _sessionidmap.find(session_id);
    if(loc == _sessionidmap.end())
    {
        _lg.strm(sl::warning) << "tried to close an invalid session";
        return false;
    };
    
    std::lock_guard<std::mutex> lck(_sessionmut);
    remove_session(loc->second);
    return true;
    
}

bool foxtrot::SessionManager::renew_session(const foxtrot::Sessionid& session_id)
{
    auto loc = _sessionidmap.find(session_id);
    if(loc == _sessionidmap.end())
    {
        _lg.strm(sl::warning) << "tried to renew an invalid session";
        return false;
    }
    
    std::lock_guard<std::mutex> lck(_sessionmut);
    auto& sesinfo = _sessionmap.at(loc->second);
    
    auto now = std::chrono::system_clock::now();
    
    _lg.strm(sl::debug) << "renewing session with id: " << loc->second;
    
    sesinfo.expiry = now + _session_length;
    return true;
}

bool foxtrot::SessionManager::session_auth_check(const foxtrot::Sessionid& secret,
                                                 const std::string& flagname) const
{
    return auth_check(secret, [] (const auto& sesinfo) {return sesinfo.flags;}, flagname);
}

bool foxtrot::SessionManager::session_auth_check(const foxtrot::Sessionid& secret, 
                                                 unsigned short devid) const
{
    return auth_check(secret, [](const auto& sesinfo) {return sesinfo.devices;}, devid);
}

void foxtrot::SessionManager::update_at_next_expiry()
{
    //find earliest expiry time;
    std::vector<time_type> expiry_times;
    expiry_times.resize(_sessionmap.size());
    std::transform(_sessionmap.begin(), _sessionmap.end(),
                   expiry_times.begin(), 
                   [] (const auto& it)
                   { return it.second.expiry;});
    
    auto next_expiry = std::min_element(expiry_times.begin(), expiry_times.end());
    
    auto next_wakeup = *next_expiry + std::chrono::seconds(1);
    
    _lg.strm(sl::debug) << "next expiry time is: " << put_time_helper(*next_expiry);
    _lg.strm(sl::debug) << "waiting til next expiry +1 second to update sessions";
    std::this_thread::sleep_until(next_wakeup);
    _lg.strm(sl::debug) << "updating sesion states...";
    update_session_states();
    
}


void foxtrot::SessionManager::update_session_states()
{
    _lg.strm(sl::trace) << "updating session state variables";
    std::lock_guard<std::mutex> lck(_sessionmut);
    auto now = std::chrono::system_clock::now();
    
    std::vector<unsigned> expired_ids;
    expired_ids.reserve(_sessionmap.size());
    
    for(const auto& [sessionind, sesinfo] : _sessionmap)
    {
        if( now > sesinfo.expiry)
        {
            //session has expired, queue for removal
            expired_ids.push_back(sessionind);
            continue;
        }
    };
    
    for(auto& id : expired_ids)
    {
        remove_session(id);
    };
    
}

const std::map<unsigned short, session_info>::const_iterator foxtrot::SessionManager::cbegin() const
{
    return _sessionmap.cbegin();
}

const std::map<unsigned short, session_info>::const_iterator foxtrot::SessionManager::cend() const
{
    return _sessionmap.cend();
}

void foxtrot::SessionManager::remove_session(unsigned short sesid)
{
    auto& sesinfo = _sessionmap.at(sesid);
    
    for(auto& devid : sesinfo.devices)
    {
        _devicecachemap.erase(devid);
    };
    
    for(auto& flagname : sesinfo.flags)
    {
        _flagcachemap.erase(flagname);
    }
    

    auto loc = std::find_if(_sessionidmap.begin(), _sessionidmap.end(),
                         [sesid] (const auto& item) {return item.second == sesid;});
    _sessionidmap.erase(loc);
    _sessionmap.erase(sesid);
}

foxtrot::Sessionid foxtrot::SessionManager::generate_session_id()
{
    Sessionid out;
    randombytes_buf(out.data(), out.size());
    return out;
}
