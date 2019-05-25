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


foxtrot::ft_argtype parse_wire_arg(const foxtrot::capability_argument& inarg, bool& success, foxtrot::Logging* lg = nullptr)
{
    success = true;
    foxtrot::ft_argtype outarg;
    auto which_type = inarg.value_case();
    
    switch(which_type)
    {
        case(capability_argument::ValueCase::kDblarg):
            outarg = inarg.dblarg();
            if(lg)
                lg->strm(sl::trace) << "argument is a double";
            break;
        case(capability_argument::ValueCase::kIntarg):
            outarg = inarg.intarg();
            if(lg)
                lg->strm(sl::trace) << "argument is an int";
            break;
        case(capability_argument::ValueCase::kBoolarg):
            outarg = inarg.boolarg();
            if(lg)
                lg->strm(sl::trace) << "argument is a bool";
            break;
        case(capability_argument::ValueCase::kStrarg):
            outarg = inarg.strarg();
            if(lg)
                lg->strm(sl::trace) << "argument is a string";
            break;
        case(capability_argument::ValueCase::VALUE_NOT_SET):
            success = false;
    }
    return outarg;
};

class ftreturn_visitor : public boost::static_visitor<>
{
public:
    ftreturn_visitor(foxtrot::capability_response& resp) : resp_(resp) {};
    void operator()(double& i) const
    {
        resp_.set_dblret(i);
    }
    void operator()(int& i ) const
    {
        resp_.set_intret(i);
    }
    void operator()(bool& i) const
    {
        resp_.set_boolret(i);
    }
    void operator()(const std::string& s) const
    {
        resp_.set_stringret(s);
    }
    
private:
    foxtrot::capability_response& resp_;
    
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
//     std::vector<foxtrot::ft_argtype> ftargs;
//     ftargs.reserve(req.args().size());
    
    for(auto& inarg: req.args())
    {
        bool success = false;
        auto outarg = wire_arg_to_variant(inarg, success, &_lg);
//         auto outarg = parse_wire_arg(inarg, success, &_lg);
        if(!success)
        {
            foxtrot_server_specific_error("couldn't parse wire argument at position:" + std::to_string(inarg.position()), repl, respond, _lg, tag, error_types::unknown_error);
            return true;
        }
//         ftargs.push_back(outarg);
        vargs.push_back(outarg);
    };
    
    
    try {
    //TODO: error handling here
        auto lock = _harness.lock_device_contentious(req.devid(),req.contention_timeout());
//         auto ftretval = dev->Invoke(req.capname(),ftargs.cbegin(), ftargs.cend());
        auto ftretval = dev->Invoke(req.capname(), vargs.cbegin(), vargs.cend());

//         if(ftretval.is_initialized())
//             boost::apply_visitor(ftreturn_visitor(repl),*ftretval);
    
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
