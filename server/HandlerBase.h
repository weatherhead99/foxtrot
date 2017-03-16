#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include <iostream>
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
        : _service(service), _cq(cq), _responder(&_ctxt), _status(status::CREATE), _logic(logic)
        {
//             cout << "handlerbase constructing..." << endl;
            Proceed();
        }
        virtual void Proceed() final
        {
            if(_status == status::CREATE)
            {
            _status = status::PROCESS;   
            //TODO: dispatch on this
            
            (_service->*T::requestfunptr)(&_ctxt,&_req,&_responder,_cq,_cq,this);
            
//             cout << "request posted" << endl;
            
            }
            else if(_status == status::PROCESS)
            {
	       new HandlerBase<T>(_service, _cq,_logic);
	      if(_logic.HandleRequest(_req,_reply, _responder, this))
	      {
	      _status = status::FINISH;
	      };
	      
            }
            else
            {
//                 cout << "request finished, deleting.." << endl;
                GPR_CODEGEN_ASSERT(_status == status::FINISH);
                delete this;
            }
       
        }
        
        grpc::ServerCompletionQueue* GetCQ()
	{
	  return _cq;
	}
        
    private:
            
        grpc::ServerCompletionQueue* _cq;
        grpc::ServerContext _ctxt;
        exptserve::AsyncService* _service;
        
        T& _logic;
        
        typename T::reqtp _req;
        typename T::repltp _reply;
        
	typename T::respondertp _responder;
        enum class status {CREATE,PROCESS,FINISH};
        status _status;
        

    };
};
