#include <chrono>
#include <mutex>


#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>

#include "InvokeCapabilityImpl.h"
#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/typeUtil.h>

using std::cout;
using std::endl;

using namespace foxtrot;


foxtrot::InvokeCapabilityLogic::InvokeCapabilityLogic(std::shared_ptr<DeviceHarness> harness)
: _harness(harness), _lg("InvokeCapabilityLogic")
{
}

void voidfun()
{
    
};


bool foxtrot::InvokeCapabilityLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
{
    _lg.Debug("processing invoke capability request" );

    foxtrot::Device* dev = nullptr;
    //validity of devid and capid gets checked in here

    foxtrot::Capability cap;
    
    try
      {
      cap = foxtrot::capability_id_name_handler(req, repl, *_harness, _lg, dev);
      }
    catch(...)
      {
	foxtrot_rpc_error_handling(std::current_exception(), repl, respond, _lg, tag);
	_lg.Trace("returned from error handling on capid");
	return true;
      }
	  
	  

    if(cap.type == CapabilityType::STREAM)
    {
        foxtrot_server_specific_error("tried to call Invoke on a streaming method: " + req.capname(), repl, respond, _lg, tag, error_types::ft_ServerError);
        return true;
    }
    
    std::vector<rttr::variant> vargs;
    vargs.reserve(req.args().size());

    _lg.strm(sl::trace) << "args size: " << req.args().size();
    auto targetargit = cap.Argtypes.begin();
    for(auto& inarg: req.args())
    {
        bool success = false;
        if(targetargit == cap.Argtypes.end())
            throw std::out_of_range("ran out of arg types to deduce. Possibly you supplied too many arguments");
        
        auto target_tp = *(targetargit++);
        rttr::variant outarg;

        try{
            outarg = wire_type_to_variant(inarg.value(), target_tp, &_lg);
            _lg.strm(sl::trace) << "is outarg valid? " << outarg.is_valid() ;
        }
        catch(...)
        {
            foxtrot_rpc_error_handling(std::current_exception(), repl, respond, _lg, tag);
            return true;
        }

        vargs.push_back(outarg);
    };
    
    try {
    //TODO: error handling here
      _lg.strm(sl::trace) << "locking device now.";
        auto lock = _harness->lock_device_contentious(req.devid(),req.contention_timeout());
	_lg.strm(sl::trace) << "doing invocation call";
	if(dev == nullptr)
	  throw std::logic_error("nullptr device this should NEVER HAPPEN!");
        auto ftretval = dev->Invoke(cap, vargs.cbegin(), vargs.cend());

        set_retval_from_variant(ftretval, repl, &_lg);

        respond.Finish(repl,grpc::Status::OK,tag);
        return true;

    }
    catch(...)
    {
      foxtrot_rpc_error_handling(std::current_exception(),repl,respond,_lg,tag);
      _lg.Trace("returned from error handling");
      return true;
    };


}
