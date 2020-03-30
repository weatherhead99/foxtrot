#include <foxtrot/server/interceptors/sessioninterceptor.hh>
#include <foxtrot/foxtrot.pb.h>

using namespace foxtrot;

SessionInterceptor::SessionInterceptor(SessionInterceptMessageType mtyp, shared_ptr<SessionManager> sesman)
: _sesman(sesman), _lg("SessionInterceptor"), _mtyp(mtyp)
{
    
};

void SessionInterceptor::Intercept(InterceptorBatchMethods* methods)
{
    if(methods->QueryInterceptionHookPoint(
        InterceptionHookPoints::POST_RECV_INITIAL_METADATA))
    {
        //pull out and save the session id if there is one
        auto* map = methods->GetRecvInitialMetadata();
        auto secit = map->find("session_secret");
        if(secit != map->end())
        {
            _lg.strm(sl::debug) << "got session secret";
            //TODO: is there a nicer way to do this from a grpc string_ref?
            _session_secret.resize(secit->second.size());
            std::copy(secit->second.begin(), secit->second.end(),
                      _session_secret.begin());
        }
        else
        {
            _lg.strm(sl::debug) << "no session secret provided";
        }
    }
    if(methods->QueryInterceptionHookPoint(
        InterceptionHookPoints::POST_RECV_MESSAGE))
    {
        _lg.strm(sl::trace) << "processing received message";
        switch(_mtyp)
        {
            case(SessionInterceptMessageType::Flag):
            {
                auto* msg = check_cast_message<foxtrot::serverflag>(methods);
                if(!_sesman->who_owns_flag(msg->flagname()).has_value())
                {
                    _lg.strm(sl::trace) << "no session owns flag...";
                }
                else
                {
                    _lg.strm(sl::trace) << "flag is owned...";
                }
                break;
            }   
            case(SessionInterceptMessageType::CapabilityRequest):
            {
                auto* msg = check_cast_message<foxtrot::capability_request>(methods);
                if(!_sesman->who_owns_device(msg->devid()).has_value())
                {
                    _lg.strm(sl::trace) << "no session owns device...";
                }
                else
                {
                    _lg.strm(sl::trace) << "device is owned";
                    if(_session_secret.size() == 0)
                    {
                        _lg.strm(sl::error) << "don't have session secret, must be invalid";
                    }
                    
                }
                break;
            }   
            case(SessionInterceptMessageType::FetchData):
            {
                auto* msg = check_cast_message<foxtrot::chunk_request>(methods);
                break;
            }
        }
        // check the session is valid or not
    }
    methods->Proceed(); 
}


SessionInterceptorFactory::SessionInterceptorFactory(shared_ptr<SessionManager> sesman)
: _sesman(sesman), _lg("SessionInterceptorFactory")
{
}

Interceptor * SessionInterceptorFactory::CreateServerInterceptor(ServerRpcInfo* info)
{
    std::string methodname = info->method();
    
    _lg.strm(sl::debug) << "method name: " << methodname;
    
    //TODO: hold this information in implementations of calls somehow?
    if(methodname == "/foxtrot.exptserve/InvokeCapability" )
    {
        _lg.strm(sl::debug) << "intercepting invoke capability ";
        return new SessionInterceptor(SessionInterceptMessageType::CapabilityRequest, _sesman);
    }
    else if(methodname == "/foxtrot.exptserve/FetchData")
    {
        _lg.strm(sl::debug) << "intercepting Fetch Data";
        return new SessionInterceptor(SessionInterceptMessageType::FetchData, _sesman);
    }
    else if(methodname == "/foxtrot.exptserve/SetServerFlag" 
        || methodname == "/foxtrot.exptserve/DropServerFlag")
    {
        _lg.strm(sl::debug) << "intercepting set /drop flag request";
        return new SessionInterceptor(SessionInterceptMessageType::Flag, _sesman);
    }
    
    return nullptr;
}

