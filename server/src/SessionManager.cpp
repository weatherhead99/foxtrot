#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/server/auth_utils.h>
#include <foxtrot/ContentionError.h>
#include <sodium.h>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <sstream>

using namespace foxtrot;

auto put_time_helper(const time_type& time, const string& format="%c")
{
    auto tt = std::chrono::system_clock::to_time_t(time);
    auto localtime = std::localtime(&tt);
    return std::put_time(localtime, format.c_str());
}

std::string secret_to_string(const foxtrot::Sessionid& secret)
{
    std::ostringstream oss;
    for(auto& bt : secret)
    {
        oss << int(bt) << ", ";
    }
    return oss.str();
}


foxtrot::SessionManager::SessionManager(const duration_type& session_length)
: _lg("SessionManager"), _session_length(session_length)
{
    start_updates();
}

foxtrot::SessionManager::~SessionManager()
{
    stop_updates();
}


foxtrot::ft_session_info  foxtrot::SessionManager::get_session_info(const foxtrot::Sessionid& session_id)
{
    std::shared_lock lck(_sessionmut);
    auto id = _sessionidmap.find(session_id);
    if(id == _sessionidmap.end())
    {
        _lg.strm(sl::trace) << "secret received: " << secret_to_string(session_id) ;
        
        for(auto& [id, ses] : _sessionidmap)
        {
            _lg.strm(sl::trace) << "session id: " << ses << "secret: " << secret_to_string(id);
        }
        
        
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


void foxtrot::SessionManager::notify_session_update()
{
    _lg.strm(sl::trace) << "attempting to lock update mutex";
    std::unique_lock lck(_sessionop_mut);
    _lg.strm(sl::trace) << "notifying session update thread...";
    _sessionop_cv.notify_one();
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
        check_requested_expiry(*requested_expiry);
    };
    
    
    //setup the next session internal id to be issued
    using Mt = typename decltype(_sessionmap)::value_type;
    
    //do not move this lock guard up otherwise it conflicts with shared lock in check_requested_items
    bool start_needed = true;
    
    std::lock_guard lck(_sessionmut);
    if(not _sessionmap.empty())
    {
        auto it = std::max_element(_sessionmap.cbegin(), _sessionmap.cend(), 
                                    [] (const Mt& first, const Mt& second) 
                                    {
                                        return first.first < second.first;
                                    });
        next_session_id = it->first + 1;
        start_needed = false;
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
    
    notify_session_update();

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
    notify_session_update();
    return true;
    
}

bool foxtrot::SessionManager::renew_session(const foxtrot::Sessionid& session_id,
    const time_type* const requested_expiry)
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
    
    if(requested_expiry)
    {
        _lg.strm(sl::debug) << "processing requested expiry time";
        check_requested_expiry(*requested_expiry);
        sesinfo.expiry = *requested_expiry;
    }
    else
    {
        sesinfo.expiry = now + _session_length;
    }
    
    _lg.strm(sl::debug) << "new session expiry time is: " << put_time_helper(sesinfo.expiry);
    
    notify_session_update();
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

bool foxtrot::SessionManager::update_at_next_expiry(std::unique_lock<std::mutex>* lckptr)
{
    _lg.strm(sl::trace) << "update_at_next_expiry";
    //find earliest expiry time;
    auto next_expiry = std::chrono::system_clock::now();
    {
        _lg.strm(sl::trace) << "locking session mutex";
        std::shared_lock lck(_sessionmut);
        _lg.strm(sl::trace) << "acquired lock";
        if(_sessionmap.size() == 0)
        {
            _lg.strm(sl::trace) << "no sessions";
            return false;
        }
        
        auto next_expiry_it = std::min_element(_sessionmap.begin(), _sessionmap.end(),
                                       [] (const auto& t1, const auto& t2)
                                       {
                                           return t1.second.expiry < t2.second.expiry;
                                       });
        next_expiry = next_expiry_it->second.expiry;
    }

    _lg.strm(sl::debug) << "next expiry: " << put_time_helper(next_expiry);
    auto now = std::chrono::system_clock::now();
    
    
    if(now > next_expiry)
    {
        _lg.strm(sl::debug) << "now is already after expiry";
        _lg.strm(sl::debug) << "now is: " << put_time_helper(now);
        _lg.strm(sl::debug) << "next expiry is: " << put_time_helper(next_expiry);
        update_session_states();
    }
    else
    {
        auto next_wakeup = next_expiry + std::chrono::seconds(1);
        _lg.strm(sl::debug) << "next expiry time is: " << put_time_helper(next_expiry);
        _lg.strm(sl::debug) << "waiting til next expiry +1 second to update sessions";
        auto stat = _stop_updates_cv.wait_until(*lckptr, next_wakeup);
        _lg.strm(sl::debug) << "updating sesion states...";
        update_session_states();
    }
    
    return true;
}


void foxtrot::SessionManager::start_updates()
{
    _lg.strm(sl::info) << "starting session update thread";
    
    _stop_updates = false;
    
//     if(_update_thread.joinable())
//     {
//         _lg.strm(sl::debug) << "update thread is joinable, joining it...";
//         _update_thread.join();
//         _lg.strm(sl::debug) << "proceeding";
//     }
    
     _update_thread = std::move(std::thread(
        [this] () {
            bool localstopupdates = false;
            while(not localstopupdates)
            {
                try{
                    std::unique_lock lck(_updatemut);
                    localstopupdates = _stop_updates;
                    if(!update_at_next_expiry(&lck))
                    {
                        _lg.strm(sl::trace) << "waiting until update notification";
                        std::unique_lock lck(_sessionop_mut);
                        _sessionop_cv.wait(lck);
                    }
                }
                catch(...)
                {
                    _stop_updates = true;
                    _lg.strm(sl::fatal) << "an exception happened in start_updates";
                }
            }
        }));

    
}

void foxtrot::SessionManager::stop_updates()
{
    _lg.strm(sl::info) << "stopping session update thread";
    {
        std::lock_guard lck(_updatemut);
        _stop_updates = true;
        _stop_updates_cv.notify_all();
    }
    _lg.strm(sl::debug) << "waiting for update thread to finish;";
    if(_update_thread.joinable())
        _update_thread.join();
    
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
    
    _lg.strm(sl::info) << "session for user: " << sesinfo.user_identifier << "has expired";
    
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

void foxtrot::SessionManager::check_requested_expiry(const time_type& tm)
{
    auto now = std::chrono::system_clock::now();
    if(tm > (now + _session_length))
    {
        _lg.strm(sl::error) << "requested a session longer than max allowed";
        throw std::out_of_range("requested a session longer than max allowed");
        
    }
    else if(tm < now)
    {
        _lg.strm(sl::error) << "now: " << put_time_helper(now);
        _lg.strm(sl::error) << "requested expiry time: " << put_time_helper(tm);
        _lg.strm(sl::error) << "requested a session expiry in the past";
        throw std::out_of_range("requested a session expiry in the past");
    }
};
