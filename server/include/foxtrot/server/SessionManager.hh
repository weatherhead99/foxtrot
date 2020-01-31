#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <array>
#include <vector>
#include <optional>
#include <iterator>
#include <mutex>
#include <foxtrot/Logging.h>
#include <boost/functional/hash.hpp>
#include <chrono>

using std::string;
using std::vector;
using std::optional;

using time_type = std::chrono::time_point<std::chrono::system_clock>;
using duration_type = typename time_type::duration;


const unsigned short SESSION_ID_BYTES = 8;

namespace foxtrot
{
    
    struct ft_session_info
    {
        unsigned short internal_session_id;
        string user_identifier;
        string comment;
        vector<unsigned> devices;
        vector<string> flags;
        time_type expiry;
    };
    
    using Sessionid = std::array<unsigned char, SESSION_ID_BYTES>;
    
    struct Sessionid_hash
    {
        auto operator()(const Sessionid& id) const
        {
            return boost::hash_range(id.begin(), id.end());
        }
    };
    
    
    class SessionManager
    {
    public:
        SessionManager(const duration_type& session_length);
        
        void update_session_states();
        bool session_auth_check(const Sessionid& secret, unsigned devid) const;
        bool session_auth_check(const Sessionid& secret, const string& flagname) const;
        
        const ft_session_info* const who_owns_device(unsigned devid) const;
        const ft_session_info* const who_owns_flag(const string& flagname) const;
        
        Sessionid start_session(const string& username, 
                                          const string& comment,
                                          const vector<unsigned>* const devices_requested, 
                                          const vector<string>* const flags_requested);
        
        bool renew_session(const Sessionid& session_id);
        bool close_session(const Sessionid& session_id);
        
        const ft_session_info& get_session_info(const Sessionid& session_id) const;
        const ft_session_info& get_session_info(unsigned short id) const;
        
        const std::map<unsigned short, ft_session_info>::const_iterator cbegin() const;
        const std::map<unsigned short, ft_session_info>::const_iterator cend() const;
        
    private:
        template<typename T, typename F> void check_requested_items(T* req, F checkfun)
        {
            if(req)
            {
                for(auto& item : *req)
                {
                    const ft_session_info* const sesinfo = checkfun(item);
                    if(sesinfo != nullptr)
                    {
                        throw sesinfo->internal_session_id;
                    }
                }
            }
        };
        
        template<typename M>
        const ft_session_info* const find_in_cache(const M& map, const typename M::key_type& val) const
        {
            auto it = map.find(val);
            if(it != map.end())
            {
                return &_sessionmap.at(it->second);
            }
            return nullptr;
        }
        
        template<typename F, typename V>
        bool auth_check(const Sessionid& secret, const F& valmap_get, const V& val) const
        {
            //check if session id is valid at all
            auto loc = _sessionidmap.find(secret);
            if(loc == _sessionidmap.end())
                return false;
            
            //if so, check if requested flag is in the session_info
            auto sesinfo = _sessionmap.at(loc->second);
            
            auto valmap = valmap_get(sesinfo);
            auto iloc = std::find(valmap.begin(), valmap.end(), val);
            if(iloc == valmap.end())
                return false;
            
            return true;
        }
        
        void remove_session(unsigned short sesid);
        Sessionid generate_session_id();
        
        void update_at_next_expiry();
        
        
        std::map<unsigned short, ft_session_info> _sessionmap;
        std::unordered_map<Sessionid, unsigned short, Sessionid_hash> _sessionidmap;
        std::map<unsigned short, unsigned> _devicecachemap;
        std::unordered_map<string, unsigned short> _flagcachemap;
        
        std::mutex _sessionmut;
        
        unsigned short next_session_id = 0;
        
        foxtrot::Logging _lg;
        duration_type _session_length;
    };
    
};
