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
            
            std::vector<rttr::argument> callargs;
            try{
	    callargs = get_callargs(meth,req,repl);
	    }
	    catch(int& i)
	    {
	      return;
	    }
            
            auto& mut = _harness.GetMutex(devid);
            std::lock_guard<std::mutex> lock(mut);
	    
	    
            retval = meth.invoke_variadic(*dev,callargs);
                        
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
