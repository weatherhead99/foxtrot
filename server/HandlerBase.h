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
	virtual bool newcall() = 0;
    };
    
    template<typename T> class HandlerBase : public HandlerTag
    {
      
    public:
        
        HandlerBase(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq, T& logic)
        : _service(service), _cq(cq), _responder(&_ctxt),  _logic(logic),
        _lg("HandlerBase")
        {
	  _status = newcall() ? status::CREATE : status::PROCESS;
	  
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
	       //NOTE: only need another handler if it's a newcall, otherwise
	      //this one will be used many times
		if(newcall())
		{
		  new HandlerBase<T>(_service, _cq,_logic);
		}
		
                if(request_status = _logic.HandleRequest(_req,_reply, _responder, this))
                {
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
        
        virtual bool newcall() final
        {
	  return T::newcall;
	};
        
        
        grpc::ServerCompletionQueue* GetCQ()
        {
        return _cq;
        }
        
    private:
        bool _newcall;
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
	bool request_status = true;

    };
};
