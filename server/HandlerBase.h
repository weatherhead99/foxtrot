#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include <iostream>
#include <type_traits>
#include "Logging.h"
using std::cout;
using std::endl;

namespace foxtrot
{
    class HandlerTag
    {
    public:
        virtual void Proceed() = 0;
    };
    
    template<typename T> class HandlerBase : public HandlerTag
    {
      
    public:
        
        HandlerBase(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq, T& logic)
        : _service(service), _cq(cq), _responder(&_ctxt),  _logic(logic), _status(status::CREATE),
        _lg("HandlerBase")
        {
            Proceed();
        }
        virtual void Proceed() final
        {
            if(_status == status::CREATE)
            {       
                _lg.Debug("request status is CREATE, processing");
                _status = status::PROCESS;   
                //TODO: dispatch on this
            
                (_service->*T::requestfunptr)(&_ctxt,&_req,&_responder,_cq,_cq,this);
            }
            else if(_status == status::PROCESS)
            {
	       
                if(_newrequest)
                {
                    _lg.Debug("new request, spawning new handler");
                    new HandlerBase<T>(_service, _cq,_logic);
                    _newrequest = false;
                }
		
                if(_logic.HandleRequest(_req,_reply, _responder, this))
                {
                    _lg.Debug("request successful, marking finished");
                    _status = status::FINISH;
                };
            
            }
            else
            {
                _lg.Debug("request finished, deleting HandlerBase");
                GPR_CODEGEN_ASSERT(_status == status::FINISH);
                delete this;
            }
       
        }
        
        grpc::ServerCompletionQueue* GetCQ()
        {
        return _cq;
        }
        
    private:
        bool _newrequest = true;
        grpc::ServerCompletionQueue* _cq;
        grpc::ServerContext _ctxt;
        exptserve::AsyncService* _service;
        
        T& _logic;
        
        typename T::reqtp _req;
        typename T::repltp _reply;
        
        typename T::respondertp _responder;
        enum class status {CREATE,PROCESS,FINISH};
        status _status;
        
        foxtrot::Logging _lg;

    };
};
