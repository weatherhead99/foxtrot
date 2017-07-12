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
        
        HandlerBase(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq, std::shared_ptr<T> logic)
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
	       _status = status::IN_FLIGHT;
	      _lg.Debug("request id is: " + std::to_string((long unsigned) this));
	      if(_newrequest)
	      {
		  _lg.Debug("spawning new handler");
		  new HandlerBase<T>(_service, _cq,_logic);
		  _newrequest = false;
	      }
	      
              if(_logic->HandleRequest(_req,_reply, _responder, this))
              {
		  
                    _lg.Debug("request successful, marking finished");
                    _status = status::FINISH;
		    
              }
              else
	      {
		  _lg.Debug("request not finished yet");
		  _status = status::PROCESS;
		  
	      }
            
            }
            else if (_status == status::IN_FLIGHT)
	    {
	      _lg.Debug("already in flight, skipping...");
	    }
            else
            {
	      _lg.Debug("finishing request");
	      
                _lg.Debug("request finished, deleting HandlerBase");
                GPR_CODEGEN_ASSERT(_status == status::FINISH);
                delete this;
            }
       
        }
        
        void FinishRequest(typename T::respondertp& respond, typename T::repltp& repl)
	{
	  constexpr bool twoargs = std::is_base_of<grpc::ServerAsyncWriter<typename T::repltp>, typename T::respondertp>::value;
	
	  //NOTE: should be constexpr as well?
	  if(twoargs)
	  {
	    respond.Finish(grpc::Status::OK,this);
	  }
	  else
	  {
	    respond.Finish(repl,grpc::Status::OK,this);
	  
	  }
	  
	};
        
	
    private:
      
	
        std::shared_ptr<T> _logic;
        bool _newrequest = true;
        grpc::ServerCompletionQueue* _cq;
        grpc::ServerContext _ctxt;
        exptserve::AsyncService* _service;
        
        
        
        typename T::reqtp _req;
        typename T::repltp _reply;
        
        typename T::respondertp _responder;
        enum class status {CREATE,PROCESS,IN_FLIGHT,FINISH};
        status _status;
        
        foxtrot::Logging _lg;

    };
};
