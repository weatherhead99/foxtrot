#pragma once
#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/Logging.h>
#include <foxtrot/ServerError.h>
#include <memory>
#include <utility>

using std::shared_ptr;


namespace foxtrot
{
    namespace detail
    {
    
        template<typename, typename = void>
        struct has_devid : std::false_type{};
        
        template<typename T>
        struct has_devid<T, std::void_t<decltype(std::declval<T>().devid())>>
        : std::true_type {};
        
        
        template<typename, typename = void>
        struct has_flagname: std::false_type{};
        
        template<typename T>
        struct has_flagname<T, std::void_t<decltype(std::declval<T>().flagname())>>
        : std::true_type{};
        
        
        template<typename R>
        bool check_session_needed(const R& req, SessionManager& sesman, 
                                  DeviceHarness& harness, Logging& lg)
        {
            if constexpr(has_devid<R>::value)
            {
                lg.strm(sl::debug) << "has devid!";
                
                auto cap = harness.GetDevice(req.devid())->GetCapability(req.capname());
                
                //disable this for now, think about how to make this work in future
                
                
//                 //capability is readonly, doesn't affect other user to read it
//                 if(cap.type == CapabilityType::VALUE_READONLY)
//                     return false;
//                 
//                 //capability is readwrite, but we are reading it
//                 if(cap.type == CapabilityType::VALUE_READWRITE && req.args().size() == 0)
//                     return false;
//                 
                auto optsession = sesman.who_owns_device(req.devid());
                if(optsession.has_value())
                    return true;
                return false;
            }
            else if constexpr(has_flagname<R>::value)
            {
                lg.strm(sl::debug) << "has flagname!";
                auto optsession = sesman.who_owns_flag(req.flagname());
                if(optsession.has_value())
                    return true;
                return false;
            }
            else
            {
                lg.strm(sl::warning) << "generic!";
                return false;
            }
            
        }
        
        
    };
    
    
    template<typename T>
    class SessionHandlerMixin : public T
    {
    public:
        template<typename ...Ts>
        SessionHandlerMixin(shared_ptr<SessionManager> sesman, shared_ptr<DeviceHarness> devharness, Ts... args)
        : _sesman(sesman), _devharness(devharness), _lg("SessionHandlerMixin"),
        T(std::forward<Ts...>(args...))
        {
        };
        
        bool check_metadata(grpc::ServerContext& ctxt, typename T::reqtp& req)
        {
            bool parent_check = T::check_metadata(ctxt, req);
            //no need to do expensive checking if some other mixin has already said no
            if(!parent_check)
                return false;
            
            //invoke parent check_metadata
            _lg.strm(sl::trace) << "SessionHandlerMixin processing metadata";
            
            //check if we require a session auth
            if(detail::check_session_needed(req,*_sesman, *_devharness, _lg))
            {
                _lg.strm(sl::debug) << "need session auth";
                auto id = get_request_session_id(req, ctxt);
                if(!_sesman->session_auth_check(id, req.devid()))
                {
                    _lg.strm(sl::error) << "invalid secret to run command";
                    throw foxtrot::ServerError("invalid session secret");
                }
                return true;
            }
            else
            {
                _lg.strm(sl::debug) << "don't need session auth";
                return true;
            }
            
            
            return true;  
        };
        
        
        
    private:
        foxtrot::Sessionid get_request_session_id(typename T::reqtp& req, grpc::ServerContext& ctxt)
        {
            auto metadat = ctxt.client_metadata();
            
            for(auto& [k,v] : metadat)
            {
                _lg.strm(sl::info) << "MD: " << k << ": " << v;
            }
            
            auto secpos = metadat.find("session_secret-bin");
            if(secpos == metadat.end())
            {
                _lg.strm(sl::error) << "no session secret provided, but it is needed";
                throw foxtrot::ServerError("request locked by user session, but no secret provided");
            }
            
            foxtrot::Sessionid out;
            
            if(secpos->second.size() != out.size())
            {
                _lg.strm(sl::error) << "wrong size for session secret, got: " << 
                secpos->second.size() << " expected: " << out.size();
                throw std::out_of_range("invalid session secret array size");
            }
            
            std::copy(secpos->second.begin(), secpos->second.end(), out.begin());
            
            return out;
        };
        
        foxtrot::Logging _lg;
        shared_ptr<SessionManager> _sesman;
        shared_ptr<DeviceHarness> _devharness;
        
    };
    
}
