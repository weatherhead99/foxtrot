#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include <iostream>
#include <type_traits>
#include "Logging.h"
#include "HandlerTag.h"
#include "ServerUtil.h"

using std::cout;
using std::endl;

namespace detail
{
    template<int> struct int_helper {
        
    };
    
    template <> struct int_helper<4> {
        typedef unsigned long type;
    };
    
    template<> struct int_helper<8> {
        typedef unsigned long long type;
    };
    
    typedef typename int_helper<sizeof(void*)>::type pointer_print_type;
    
    
}

namespace foxtrot
{
    
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
            if(_status == status::CREATE || _status == status::PROCESS)
            {
            
                std::lock_guard<std::mutex> lk(_procmut);
                if(_status == status::CREATE)
                {       
                    _lg.Debug("request status is CREATE, processing");
                    _status = status::PROCESS;   
                    //TODO: dispatch on this
                
                    (_service->*T::requestfunptr)(&_ctxt,&_req,&_responder,_cq,_cq,this);
                    
                }
                else if(_status == status::PROCESS)
                {
                    _lg.strm(sl::trace) << "request id is: " << reinterpret_cast<detail::pointer_print_type>(this);
                    if(_newrequest)
                    {
                        _lg.Debug("spawning new handler");
                        new HandlerBase<T>(_service, _cq,_logic);
                        _newrequest = false;
                    }
                    try{
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
                    catch(...)
                    {
                        _lg.Info("Handlerbase error handler invoked...");
                        auto eptr = std::current_exception();
                        foxtrot_rpc_error_handling(eptr, _reply, _responder,
                                                   _lg, this);   
                        _status = status::FINISH;
                    };
            
                }
            }
            else
            {
                _lg.Debug("finishing request");
        
                _lg.Debug("request finished, deleting HandlerBase");
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
	
	std::mutex _procmut;

    };
};
