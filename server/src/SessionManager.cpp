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



foxtrot::ft_session_info  foxtrot::SessionManager::get_session_info(const foxtrot::Sessionid& session_id)
{
    std::shared_lock lck(_sessionmut);
    auto id = _sessionidmap.find(session_id);
    if(id == _sessionidmap.end())
    {
        throw std::out_of_range("invalid secret!");
    }
    return _sessionmap.at(id->second);
    
}

foxtrot::ft_session_info foxtrot::SessionManager::get_session_info(unsigned short id)
{
    std::shared_lock lck(_sessionmut);
    return _sessionmap.at(id);
}



optional<foxtrot::ft_session_info> foxtrot::SessionManager::who_owns_device(unsigned devid)
{
    optional<ft_session_info> out;
    _lg.strm(sl::trace) << "about to shared lock session mutex";
    std::shared_lock lck(_sessionmut);
    _lg.strm(sl::trace) << "about to find in cache";
    auto ptr = find_in_cache(_devicecachemap, devid);
    
    if(ptr)
    {
        out = *ptr;
        return out;
    }
    
    return out;
}

optional<foxtrot::ft_session_info> foxtrot::SessionManager::who_owns_flag(const std::string& flagname)
{
    optional<ft_session_info> out;
    std::shared_lock lck(_sessionmut);
    auto ptr = find_in_cache(_flagcachemap, flagname);
    
    if(ptr)
    {
        out = *ptr;
        return out;
    }
    
    return out;
}


std::tuple<foxtrot::Sessionid, unsigned short> foxtrot::SessionManager::start_session(
    const std::string& username, 
    const std::string& comment, 
    const vector<unsigned> *const devices_requested, 
    const vector<std::string> *const flags_requested,
    const time_type* const requested_expiry)
{
    
    auto now = std::chrono::system_clock::now();
    _lg.strm(sl::info) << "new user session requested by: "<< username 
                        << "with comment: " << comment;
    
                        
    //check that devices and flags aren't already in a session
    try{
        _lg.strm(sl::debug) << "checking for requested devices..." ;
        check_requested_items(devices_requested, [this] (auto devid) { return who_owns_device(devid);});
        _lg.strm(sl::debug) << "checking for requested flags...";
        check_requested_items(flags_requested, [this] (auto flagname) { return who_owns_flag(flagname);});
    }
    catch(short unsigned i)
    {
        _lg.strm(sl::error) << "a requested item is owned by session with id: " << i;
        throw i;
    };
    
    if(requested_expiry)
    {
        //check that session length is allowed
        if(*requested_expiry > (now + _session_length))
        {
            _lg.strm(sl::error) << "requested a session longer than max allowed";
            throw std::out_of_range("requested session longer than max allowed");
        }
        
    };
    
    
    //setup the next session internal id to be issued
    using Mt = typename decltype(_sessionmap)::value_type;
    
    //do not move this lock guard up otherwise it conflicts with shared lock in check_requested_items
    std::lock_guard lck(_sessionmut);
    if(not _sessionmap.empty())
    {
        auto it = std::max_element(_sessionmap.cbegin(), _sessionmap.cend(), 
                                    [] (const Mt& first, const Mt& second) 
                                    {
                                        return first.first < second.first;
                                    });
        next_session_id = it->first + 1;
    }
    
    ft_session_info newsession;
    newsession.comment = comment;
    newsession.user_identifier = username;
    newsession.internal_session_id = next_session_id;
    
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
    return std::make_tuple(secret, next_session_id);
}


bool foxtrot::SessionManager::close_session(const foxtrot::Sessionid& session_id)
{
    std::lock_guard lck(_sessionmut);
    auto loc = _sessionidmap.find(session_id);
    if(loc == _sessionidmap.end())
    {
        _lg.strm(sl::warning) << "tried to close an invalid session";
        return false;
    };
    
    remove_session(loc->second);
    return true;
    
}

bool foxtrot::SessionManager::renew_session(const foxtrot::Sessionid& session_id)
{
    std::lock_guard lck(_sessionmut);
    auto loc = _sessionidmap.find(session_id);
    if(loc == _sessionidmap.end())
    {
        _lg.strm(sl::warning) << "tried to renew an invalid session";
        return false;
    }
    
    auto& sesinfo = _sessionmap.at(loc->second);
    
    auto now = std::chrono::system_clock::now();
    
    _lg.strm(sl::debug) << "renewing session with id: " << loc->second;
    
    sesinfo.expiry = now + _session_length;
    return true;
}

bool foxtrot::SessionManager::session_auth_check(const foxtrot::Sessionid& secret,
                                                 const std::string& flagname)
{
    std::shared_lock lck(_sessionmut);
    return auth_check(secret, [] (const auto& sesinfo) {return sesinfo.flags;}, flagname);
}

bool foxtrot::SessionManager::session_auth_check(const foxtrot::Sessionid& secret, 
                                                 unsigned devid)
{
    std::shared_lock lck(_sessionmut);
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
    std::lock_guard lck(_sessionmut);
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


const duration_type & foxtrot::SessionManager::get_session_length() const
{
    return _session_length;
}


