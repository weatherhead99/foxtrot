#include "InvokeCapabilityImpl.h"

#include "foxtrot.pb.h"
#include "DeviceError.h"
#include "ProtocolError.h"
#include <chrono>

#include <mutex>
#include "ServerUtil.h"


using std::cout;
using std::endl;

using namespace foxtrot;

foxtrot::InvokeCapabilityLogic::InvokeCapabilityLogic(DeviceHarness& harness)
: _harness(harness), _lg("InvokeCapabilityLogic")
{
}


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
    
    auto devtp = rttr::type::get(*dev);
    
    auto prop = devtp.get_property(req.capname().c_str());
    auto meth = devtp.get_method(req.capname().c_str());
    
    rttr::variant retval;
    
    try{
        if (!meth && !prop)
        {
	  foxtrot_server_specific_error(
	    "no matching property or method", repl,respond,_lg,tag);
	  return true;
        }
        else if (!prop)
        {
            //method
            if(!meth.is_valid())
            {
	      foxtrot_server_specific_error(
		"invalid method", repl, respond, _lg, tag);
                _lg.Error("invalid method!");
                return true;
            }
            
            std::vector<rttr::variant> args;
            try{
                args = get_callargs(meth,req,repl);	    
                for(auto& arg: args)
                {
		  rttr::variant v = arg;
		  _lg.Trace("arg: " + v.to_string());
                }
	    
                }  
                catch(int& i)
                {
		  foxtrot_server_specific_error(
		    "couldn't get callargs", repl,respond,_lg,tag);
                return true;
                }
	    std::vector<rttr::argument> callargs(args.begin(), args.end());
                
            
            //check if it's a stream data method
	    if(is_ft_call_streaming(prop))
	      {
		    foxtrot_server_specific_error( 
		    "tried to InvokeCapability on a bulk data property!", repl, respond, _lg, tag);
                    return true;
	      }
	  
	    auto lock = _harness.lock_device_contentious(devid,req.contention_timeout());
            retval = meth.invoke_variadic(*dev,callargs);
                        
        }
        
        else
        {
            //property            
	    if(is_ft_call_streaming(prop))
	      {
		    foxtrot_server_specific_error( 
		    "tried to InvokeCapability on a bulk data property!", repl, respond, _lg, tag);
                    return true;
	      }
	  
	  
                if(prop.is_readonly())
                {
		    _lg.Trace("readonly property");
		     auto lock = _harness.lock_device_contentious(devid,req.contention_timeout());
                    retval = prop.get_value(*dev);
                }
                else
                {
                    if( req.args_size() >1)
                    {
                        //ERRROR: should only have one number to set a property
		      foxtrot_server_specific_error(
			  "require only 1 argument to writable property",
			  repl, respond, _lg,tag);
		      return true;
		      
                    }
                    else if (req.args_size() == 1)
                    {
		      bool success;
		      auto arg = get_arg(req.args().Get(0),success);
		      if(!success)
		      {
			foxtrot_server_specific_error("coultn't get argument for setting property",
						      repl,respond,_lg,tag);
			return true;
			
		      }
		      
		      auto lock = _harness.lock_device_contentious(devid,req.contention_timeout());
		      prop.set_value(*dev,arg);
                    }
                    else if(req.args_size() == 0)
                    {
		      auto lock = _harness.lock_device_contentious(devid,req.contention_timeout());
                       retval  = prop.get_value(dev);
                    }
                    
                }
                
        };
        
    }
    catch(...)
    {
      foxtrot_rpc_error_handling(std::current_exception(),repl,respond,_lg,tag);
      _lg.Trace("returned from error handling");
      return true;
    };
                 
    
    set_returntype(retval,repl);
    respond.Finish(repl,grpc::Status::OK,tag);
    return true;
}
