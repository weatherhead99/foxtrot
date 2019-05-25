#include <chrono>
#include <mutex>

#include <foxtrot/foxtrot.pb.h>
#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>

#include "InvokeCapabilityImpl.h"
#include <foxtrot/server/ServerUtil.h>

using std::cout;
using std::endl;

using namespace foxtrot;

foxtrot::InvokeCapabilityLogic::InvokeCapabilityLogic(DeviceHarness& harness)
: _harness(harness), _lg("InvokeCapabilityLogic")
{
}

void voidfun()
{
    
};


bool foxtrot::InvokeCapabilityLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
{
    _lg.Debug("processing invoke capability request" );
    
    auto devid = req.devid();
    
    repl.set_msgid(req.msgid());
    repl.set_devid(req.devid());
    repl.set_capname(req.capname());
    
    foxtrot::Device* dev;
    _lg.Debug("capability requested is: " + req.capname() );
    
    try{
        dev = _harness.GetDevice(devid);    
    }
    catch(std::out_of_range& err)
    {
      foxtrot_server_specific_error(
	"invalid device id supplied", repl,respond,_lg,tag, error_types::out_of_range);
      return true;
    };
    
    auto cap = dev->GetCapability(req.capname());
    if(cap.type == CapabilityType::STREAM)
    {
        foxtrot_server_specific_error("tried to call Invoke on a streaming method: " + req.capname(), repl, respond, _lg, tag, error_types::ft_ServerError);
        return true;
    }
    
    std::vector<rttr::variant> vargs;
    vargs.reserve(req.args().size());

    for(auto& inarg: req.args())
    {
        bool success = false;
        auto outarg = wire_arg_to_variant(inarg, success, &_lg);
        if(!success)
        {
            foxtrot_server_specific_error("couldn't parse wire argument at position:" + std::to_string(inarg.position()), repl, respond, _lg, tag, error_types::unknown_error);
            return true;
        }
        vargs.push_back(outarg);
    };
    
    
    try {
    //TODO: error handling here
        auto lock = _harness.lock_device_contentious(req.devid(),req.contention_timeout());
        auto ftretval = dev->Invoke(req.capname(), vargs.cbegin(), vargs.cend());

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
