#include "InvokeCapabilityImpl.h"

#include "foxtrot.pb.h"
#include "DeviceError.h"
#include "ProtocolError.h"

#include "devices/dummy/dummyDevice.h"

#include <mutex>
#include "ServerUtil.h"


using std::cout;
using std::endl;

using namespace foxtrot;

foxtrot::InvokeCapabilityLogic::InvokeCapabilityLogic(DeviceHarness& harness)
: _harness(harness), _lg("InvokeCapabilityLogic")
{
}


bool foxtrot::InvokeCapabilityLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, void* tag)
{
    _lg.Debug("processing invoke capability request" );
    
    auto devid = req.devid();
    repl.set_msgid(req.msgid());
    repl.set_devid(req.devid());
    repl.set_capname(req.capname());
    
    foxtrot::Device* dev;
    _lg.Debug("capability requested is: " + req.capname() );
    
//     cout << "debug string" << req.DebugString() << endl;
    
    try{
        dev = _harness.GetDevice(devid);    
    }
    catch(std::out_of_range& err)
    {
      set_repl_err(repl,err,error_types::out_of_range);
      respond.Finish(repl,grpc::Status::OK,tag);
      return true;
    };
    
    auto devtp = rttr::type::get(*dev);
    
    
    auto prop = devtp.get_property(req.capname().c_str());
    auto meth = devtp.get_method(req.capname().c_str());
    
    rttr::variant retval;
    
    try{
        if (!meth && !prop)
        {
            _lg.Error("no matching property or method error" );
        errstatus* errstat = repl.mutable_err();
        errstat->set_msg("no matching property or method");
        errstat->set_tp(error_types::out_of_range);
	respond.Finish(repl,grpc::Status::OK,tag);
        return true;
        }
        else if (!prop)
        {
            //method
            if(!meth.is_valid())
            {
                _lg.Error("invalid method!");
                set_repl_err_msg(repl,"invalid method", error_types::Error);
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
                set_repl_err_msg(repl,"couldn't get callargs",error_types::Error);
                respond.Finish(repl,grpc::Status::OK,tag);
                return true;
                }
                
            auto& mut = _harness.GetMutex(devid);
            std::lock_guard<std::mutex> lock(mut);
	    
	    std::vector<rttr::argument> callargs(args.begin(), args.end());
        
        
            //check if it's a stream data method
            auto streammeta = meth.get_metadata("streamdata");
            if(streammeta.is_valid())
            {
                if(streammeta.to_bool())
                {
                    _lg.Error("tried to InvokeCapability on a bulk data method!");
                    auto msg = "tried to InvokeCapability on a bulk data method!";
                    set_repl_err_msg(repl,msg,error_types::Error);
                    respond.Finish(repl,grpc::Status::OK,tag);
                    return true;
                }
                
        
            }
        
            retval = meth.invoke_variadic(*dev,callargs);
                        
        }
        
        else
        {
            //property            
            auto streammeta = prop.get_metadata("streamdata");
            if(streammeta.is_valid())
            {
                if(streammeta.to_bool())
                {
                    _lg.Error("tried to InvokeCapability on a bulk data property!");
                    auto msg = "tried to InvokeCapability on a bulk data property!";
                    set_repl_err_msg(repl,msg,error_types::Error);
                    respond.Finish(repl,grpc::Status::OK,tag);
                    return true;
                }
                
        
            }
                if(prop.is_readonly())
                {
//                     cout << "readonly property" << endl;
                    retval = prop.get_value(*dev);
                }
                else
                {
                    if( req.args_size() >1)
                    {
                        //ERRROR: should only have one number to set a property
                        errstatus* errstat = repl.mutable_err();
                        errstat->set_msg("require only 1 argument to writeable property");
                        errstat->set_tp(error_types::ProtocolError);
                    }
                    else if (req.args_size() == 1)
                    {
                    auto& mut = _harness.GetMutex(devid);
                    std::lock_guard<std::mutex> lock(mut);
                    bool success;
                    auto arg = get_arg(req.args().Get(0),success);
                    prop.set_value(*dev,arg);
                    }
                    else if(req.args_size() == 0)
                    {
                        retval  = prop.get_value(dev);
                    }
                    
                }
                
        };
        
    }
    catch(class DeviceError& err)
        {
            _lg.Error("caught device error" );
            set_repl_err(repl,err,error_types::DeviceError);
            respond.Finish(repl,grpc::Status::OK,tag);
             return true;
         }
    catch(class ProtocolError& err)
         {
             _lg.Error("caught protocol error" );
             set_repl_err(repl,err,error_types::ProtocolError);
            respond.Finish(repl,grpc::Status::OK,tag);
             return true;
         }
    catch(std::exception& err)
    {
          _lg.Error("caught generic error" );
        set_repl_err(repl,err,error_types::Error);
        respond.Finish(repl,grpc::Status::OK,tag);
        return true;
    }
            
//     cout << "repl has error: " << repl.has_err() << endl;
//     cout << "repl return: " << repl.dblret() << endl;
         
    set_returntype(retval,repl);
    respond.Finish(repl,grpc::Status::OK,tag);
    return true;
}
