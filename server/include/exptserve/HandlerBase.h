#pragma once
#include <iostream>
#include <type_traits>
#include <grpc++/grpc++.h>

#include <proto/foxtrot.grpc.pb.h>
#include <foxtrot/Logging.h>
#include <foxtrot/server/ServerUtil.h>

#include "HandlerTag.h"

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
    
    template<typename T, typename Service=typename T::servicetp> class HandlerBase : public HandlerTag
    {
      
    public:
        
        HandlerBase(Service* service, grpc::ServerCompletionQueue* cq, std::shared_ptr<T> logic)
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
                    _lg.Trace("request status is CREATE, processing");
                    _status = status::PROCESS;   
                    //TODO: dispatch on this
                    if(!_service)
                        _lg.Error("service is nullptr");
                    if(!_cq)
                        _lg.Error("cq is nullptr");
                    if(!this)
                        _lg.Error("this is nullptr");
                    _lg.Trace("about to call requestfunptr");
		    
                    (_service->*T::requestfunptr)(&_ctxt,&_req,&_responder,_cq,_cq,this);
                    _lg.Trace("requestfunptr returned");
                    
                }
                else if(_status == status::PROCESS)
                {
                    _lg.strm(sl::trace) << "request id is: " << reinterpret_cast<detail::pointer_print_type>(this);
                    
                    _lg.strm(sl::debug) << "request peer string: " << _ctxt.peer();
                    
                    for(auto& [k,v] : _ctxt.client_metadata())
                    {
                        _lg.strm(sl::trace) << "call metadata, key: " << k << "value: " << v ;
                    }
                    
                    if(_newrequest)
                    {
                        _lg.Trace("spawning new handler");
                        new HandlerBase<T, Service>(_service, _cq,_logic);
                        _newrequest = false;
                    }
                    try{
                        if(_logic->check_metadata(_ctxt, _req))
                        {
                            _lg.Trace("request passed metadata check, continuing");
                            if(_logic->HandleRequest(_req,_reply, _responder, this))
                            {
                                _lg.Trace("request successful, marking finished");
                                _status = status::FINISH;
                            }
                            else
                            {
                                _lg.Trace("request not finished yet");
                                _status = status::PROCESS;
                            }
                        }
                        else
                        {
                            _lg.Trace("request failed metadata check, finishing");
                            _status = status::FINISH;
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
    
    
    
    
    const grpc::ServerContext& getContext() const
    {
        return _ctxt;
    }

    private:
	
        std::shared_ptr<T> _logic;
        bool _newrequest = true;
        grpc::ServerCompletionQueue* _cq;
        grpc::ServerContext _ctxt;
        Service* _service;
	
        typename T::reqtp _req;
        typename T::repltp _reply;
        
        typename T::respondertp _responder;
        enum class status {CREATE,PROCESS,IN_FLIGHT,FINISH};
        status _status;
        
        foxtrot::Logging _lg;
	
	std::mutex _procmut;

    };
};
