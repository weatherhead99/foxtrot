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
: _harness(harness)
{
}


template <typename T> bool foxtrot_error_checking(T fun, capability_response& repl)
{
             try{    
                fun();
                return true;
         }
         catch(class DeviceError& err)
         {
             set_repl_err(repl,err,error_types::DeviceError);
             return false;
         }
         catch(class ProtocolError& err)
         {
             set_repl_err(repl,err,error_types::ProtocolError);
             return false;
         }
         catch(std::out_of_range& err)
         {
             set_repl_err(repl,err,error_types::out_of_range);
             return false;
             
         }
         catch(std::exception& err)
         {
             set_repl_err(repl,err,error_types::Error);
             return false;
         }
         catch(...)
         {
             auto errstat = repl.mutable_err();
             errstat->set_msg("");
             errstat->set_tp(error_types::unknown_error);
             return false;
             
         }
         ;
             
};


bool set_returntype(rttr::variant& retval, capability_response& repl)
{
    
    cout << "setting return type" << endl;
        auto rettp = get_appropriate_wire_type(retval.get_type());
        cout << "rettp is: "<< rettp << endl;
            bool convertsuccess = true;
         if(rettp == value_types::FLOAT)
         {
             cout << "it's a double!" << endl;
            repl.set_dblret(retval.to_double(&convertsuccess));
         }
         else if (rettp == value_types::BOOL)
         {
             convertsuccess = retval.can_convert(rttr::type::get<bool>());
             repl.set_boolret(retval.to_bool());
         }
         else if(rettp == value_types::INT)
         {
             cout << "int" << endl;
             repl.set_intret(retval.to_int(&convertsuccess));
         }
         else if(rettp == value_types::STRING)
         {
             cout << "string" << endl;
             repl.set_stringret(retval.to_string(&convertsuccess));
         }
         //if it's VOID, no need to set rettp
         else if(rettp == value_types::VOID)
	 {
	    cout << "void" << endl;
	    repl.set_stringret("");
	   
	 }
         
         if(!convertsuccess)
         {
             errstatus* errstat = repl.mutable_err();
             errstat->set_msg("couldn't successfully convert return type");
             errstat->set_tp(error_types::Error);
         };
         
         cout << "convertsuccess: "<< convertsuccess << endl;
         return convertsuccess;
}


rttr::variant get_arg(const capability_argument& arg, bool& success)
{
    success=true;
    rttr::variant outarg;
    auto which_type = arg.value_case();
    cout << "arg type switch: " << which_type << endl;
    
    switch(which_type)
    {
    case(capability_argument::ValueCase::kDblarg):
        outarg = arg.dblarg();
        break;
        
    case(capability_argument::ValueCase::kIntarg):
        outarg = arg.intarg();
        break;
        
    case(capability_argument::ValueCase::kBoolarg):
        
        outarg = arg.boolarg();
        break;
    
    case(capability_argument::ValueCase::kStrarg):
        
        outarg = arg.strarg();
        break;

    case(capability_argument::ValueCase::VALUE_NOT_SET):
        success = false;
    }

//     cout << "outarg type: " << outarg.get_type().get_name() << endl;
    return outarg;
}


void foxtrot::InvokeCapabilityLogic::HandleRequest(reqtp& req, repltp& repl)
{
    std::cout << "processing invoke capability request" << std::endl;
    
    auto devid = req.devid();
    repl.set_msgid(req.msgid());
    repl.set_devid(req.devid());
    repl.set_capname(req.capname());
    
    foxtrot::Device* dev;
    cout << "capability requested is: " << req.capname() << endl;
    
//     cout << "debug string" << req.DebugString() << endl;
    
    try{
        dev = _harness.GetDevice(devid);    
    }
    catch(std::out_of_range& err)
    {
      set_repl_err(repl,err,error_types::out_of_range);
      return;
    };
    
    auto devtp = rttr::type::get(*dev);
    
    
    auto prop = devtp.get_property(req.capname().c_str());
    auto meth = devtp.get_method(req.capname().c_str());
    
    
    
    rttr::variant retval;
    
    try{
        if (!meth && !prop)
        {
            std::cout << "no matching property or method error" << std::endl;
        errstatus* errstat = repl.mutable_err();
        errstat->set_msg("no matching property or method");
        errstat->set_tp(error_types::out_of_range);
        return;
        }
        else if (!prop)
        {
            //method
            if(!meth.is_valid())
            {
                cout << "invalid method" << endl;
            }
            
            std::cout << "method..." << std::endl;
            auto args = req.args();
            
            std::vector<rttr::variant> argvec;
            argvec.resize(args.size());
            
            //check parameter infos
            auto param_infs = meth.get_parameter_infos();
            
            if(args.size() != param_infs.size())
            {
                cout << "unexpected number of arguments supplied" << endl;
                errstatus* errstat = repl.mutable_err();
                errstat->set_msg("incorrect number of arguments supplied");
                errstat->set_tp(error_types::out_of_range);
                return;
            }
            
            
            for(auto& arg: args)
            {
                bool success;
                rttr::variant outarg = get_arg(arg,success);
                
                   
                if(!success)
                {          
                    cout << "error in getting arguments..." << endl;
                    errstatus* errstat = repl.mutable_err();
                    errstat->set_msg("argument at position " + std::to_string(arg.position()) + "is not set");
                    errstat->set_tp(error_types::Error);
                        return;
                }
                
                const auto target_argtp = param_infs[arg.position()].get_type();
                if(!outarg.can_convert(target_argtp))
                {
                    cout << "error converting argument " << endl;
                    errstatus* errstat = repl.mutable_err();
                    errstat->set_msg("argument at position " + std::to_string(arg.position()) + 
                    "cannot be converted to type " + target_argtp.get_name());
                    errstat->set_tp(error_types::out_of_range);
                    return;
                    
                }
                
                if(outarg.get_type() != target_argtp)
                {
                    cout << "WARNING: need to convert an arg from " << outarg.get_type().get_name() << " to " 
                    << target_argtp.get_name() << endl;
                    
                };
                
                //NOTE: outarg.convert returns bool, f*ck you RTTR that is NOT obvious!
                success = outarg.convert(target_argtp);
		
                argvec[arg.position()] = outarg;
                
            };
            
            
            auto& mut = _harness.GetMutex(devid);
            std::lock_guard<std::mutex> lock(mut);
            
	    std::vector<rttr::argument> callargs(argvec.begin(), argvec.end());
	    
            retval = meth.invoke_variadic(*dev,callargs);
             
// 	    cout << "signature: " << meth.get_signature() << endl;
                        
        }
        
        else
        {
            //property
            
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
            cout << "caught device error" << endl;
            set_repl_err(repl,err,error_types::DeviceError);
             return;
         }
    catch(class ProtocolError& err)
         {
             cout << "caught protocol error" << endl;
             set_repl_err(repl,err,error_types::ProtocolError);
             return;
         }
    catch(std::exception& err)
    {
          cout << "caught generic error" << endl;
	  set_repl_err(repl,err,error_types::Error);
	  return;
    }
            
//     cout << "repl has error: " << repl.has_err() << endl;
//     cout << "repl return: " << repl.dblret() << endl;
         
    set_returntype(retval,repl);
    return;
}
