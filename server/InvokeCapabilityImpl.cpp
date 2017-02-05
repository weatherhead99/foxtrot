#include "InvokeCapabilityImpl.h"

#include "foxtrot.pb.h"
#include "DeviceError.h"
#include "ProtocolError.h"

using namespace foxtrot;

foxtrot::InvokeCapabilityLogic::InvokeCapabilityLogic(DeviceHarness& harness)
: _harness(harness)
{
}


bool set_returntype(rttr::variant& retval, capability_response& repl)
{
        auto rettp = get_appropriate_wire_type(retval.get_type());
            bool convertsuccess;
         if(rettp == value_types::FLOAT)
         {
            repl.set_dblret(retval.to_double(&convertsuccess));
         }
         else if (rettp == value_types::BOOL)
         {
             convertsuccess = retval.can_convert(rttr::type::get<bool>());
             repl.set_boolret(retval.to_bool());
         }
         else if(rettp == value_types::INT)
         {
             repl.set_intret(retval.to_int(&convertsuccess));
         }
         else if(rettp == value_types::STRING)
         {
             repl.set_stringret(retval.to_string(&convertsuccess));
         }
         //if it's VOID, no need to set rettp
         
         if(!convertsuccess)
         {
             errstatus* errstat = repl.mutable_err();
             errstat->set_msg("couldn't successfully convert return type");
             errstat->set_tp(error_types::Error);
         };
         
         return convertsuccess;
}



void foxtrot::InvokeCapabilityLogic::HandleRequest(reqtp& req, repltp& repl)
{
    std::cout << "processing invoke capability request" << std::endl;
    
    auto devid = req.devid();
    repl.set_msgid(req.msgid());
    repl.set_devid(req.devid());
    repl.set_capname(req.capname());
    
    foxtrot::Device* dev;
    
    try{
        dev = _harness.GetDevice(devid);    
    }
    catch(std::out_of_range& err)
    {
      errstatus* errstat = repl.mutable_err();
      errstat->set_msg(err.what());
      errstat->set_tp(error_types::out_of_range);
      return;
    };
    
    auto devtp = rttr::type::get(*dev);
    
    
    auto prop = devtp.get_property(req.capname().c_str());
    auto meth = devtp.get_method(req.capname().c_str());
    
    
    if (!meth && !prop)
    {
     errstatus* errstat = repl.mutable_err();
     errstat->set_msg("no matching property or method");
     errstat->set_tp(error_types::out_of_range);
        
    }
    else if (!prop)
    {
        //method
         auto args = req.args();
         
         std::vector<rttr::argument> argvec;
         argvec.resize(args.size());
         
         for(auto& arg: args)
         {
             rttr::variant outarg;
             auto which_type = arg.value_case();
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
                     errstatus* errstat = repl.mutable_err();
                     errstat->set_msg("argument at position " + std::to_string(arg.position()) + "is not set");
                     errstat->set_tp(error_types::Error);
                     return;
             }
             argvec[arg.position()] = outarg;
             
         };
         
         rttr::variant retval;
         try{
             
             //TODO: thread safety!!!!
         retval = meth.invoke_variadic(dev,argvec);
         }
         catch(class DeviceError& err)
         {
             errstatus* errstat = repl.mutable_err();
             errstat->set_msg(err.what());
             errstat->set_tp(error_types::DeviceError);
             return;
         }
         catch(class ProtocolError& err)
         {
             errstatus* errstat = repl.mutable_err();
             errstat->set_msg(err.what());
             errstat->set_tp(error_types::ProtocolError);
             return;
         };
         
        if(!set_returntype(retval,repl))
        {
            return;
        };
        
        
        
    }
    else
    {
        //property
         auto val = prop.get_value(dev);
         if(!set_returntype(val,repl))
         {
             return;
         }
         
         
         
    }

    return;
}
