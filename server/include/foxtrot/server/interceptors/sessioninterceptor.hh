#pragma once

#include <memory>
#include <grpcpp/support/server_interceptor.h>
#include <foxtrot/server/SessionManager.hh>
#include <foxtrot/Logging.h>

using namespace grpc::experimental;
using std::shared_ptr;

namespace foxtrot
{
    enum class SessionInterceptMessageType : unsigned char
    {
        CapabilityRequest,
        FetchData,
        Flag        
    };
    
    class SessionInterceptor : public Interceptor {
    public:
        SessionInterceptor(SessionInterceptMessageType mtyp, shared_ptr<SessionManager> sesman);
        void Intercept(InterceptorBatchMethods * methods) override;
    private:
        template <typename T> T* check_cast_message(InterceptorBatchMethods* methods)
        {
            auto* msg = static_cast<T*>(methods->GetRecvMessage());
            if(msg == nullptr)
            {
                _lg.strm(sl::fatal) << "couldn't cast message!";
                throw std::logic_error("couldn't cast message!");
            }
            
            return msg;
        }
        
        shared_ptr<SessionManager> _sesman;
        ServerRpcInfo* _rpcinfo;
        Logging _lg;
        SessionInterceptMessageType _mtyp;
        std::string _session_secret;
    };
    
    class SessionInterceptorFactory : public ServerInterceptorFactoryInterface
    {
    public:
        SessionInterceptorFactory(shared_ptr<SessionManager> sesman);
        virtual Interceptor* CreateServerInterceptor(ServerRpcInfo* info) override;
    private:
        shared_ptr<SessionManager> _sesman;
        Logging _lg;
    };
    
    
}
