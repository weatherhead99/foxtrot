#pragma once
#include "foxtrot.grpc.pb.h"
#include <rttr/type>
#include "Logging.h"
// #include "byteview.h"

namespace rttr
{
 class variant;   
}

namespace foxtrot
{
    template <typename replT> void set_repl_err_msg(replT& repl, const std::string& msg, error_types etype )
    {
      auto errstat = repl.mutable_err();
      errstat->set_msg(msg);
      errstat->set_tp(etype);
    }
    
    
    template <typename replT, typename errT> void set_repl_err(replT& repl, errT& err, error_types etype)
    {
        auto errstat = repl.mutable_err();
        errstat->set_msg(err.what());
        errstat->set_tp(etype);
    }
    
    
    
    template <typename retT, typename reqT> retT init_chunk(reqT& req)
    {
      retT out;
      out.set_msgid(req.msgid());
      out.set_devid(req.devid());
      out.set_capname(req.capname());
      
      return out;
      
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


    std::unique_ptr<unsigned char[]> byte_view_data(rttr::variant& arr, unsigned& byte_size, foxtrot::byte_data_types& dtype);


    rttr::variant get_arg(const capability_argument& arg, bool& success);
    
    bool set_returntype(rttr::variant& retval, foxtrot::capability_response& repl);
    value_types get_appropriate_wire_type(const rttr::variant& vr);
    
    template <typename reqtp, typename repltp> std::vector<rttr::variant> get_callargs(rttr::method& meth, 
									reqtp& req,
									repltp& repl)
    {
      foxtrot::Logging lg("get_callargs");
      
      
      auto args = req.args();
      std::vector<rttr::variant> argvec;
      argvec.resize(args.size());
      
	    //check parameter infos
      auto param_infs = meth.get_parameter_infos();
      
      if(args.size() != param_infs.size())
      {
	  lg.Error("unexpected number of arguments supplied");
	  errstatus* errstat = repl.mutable_err();
	  errstat->set_msg("incorrect number of arguments supplied");
	  errstat->set_tp(error_types::out_of_range);
	  
	  throw 1;
	  
      }
      
      for(auto& arg: args)
      {
	  bool success;
	  rttr::variant outarg = get_arg(arg,success);
	  
	      
	  if(!success)
	  {          
	      lg.Error("error in getting arguments...");
	      auto msg = "argument at position " + std::to_string(arg.position()) + "is not set";
	      set_repl_err_msg(repl,msg,error_types::Error);
	      throw 1;
	  }
	  
	  const auto target_argtp = param_infs[arg.position()].get_type();
	  if(!outarg.can_convert(target_argtp))
	  {
	      lg.Error("error converting argument ");
	      auto msg = "argument at position " + std::to_string(arg.position()) + 
	      "cannot be converted to type " + target_argtp.get_name();
	      set_repl_err_msg(repl,msg, error_types::out_of_range);
	      throw 1;
	      
	  }
	  
	  if(outarg.get_type() != target_argtp)
	  {
	      lg.Warning(" need to convert an arg from " + outarg.get_type().get_name() + " to " 
	       + target_argtp.get_name() );
	  };
	  
	  //NOTE: outarg.convert returns bool, f*ck you RTTR that is NOT obvious!
	  success = outarg.convert(target_argtp);
	  
	  argvec[arg.position()] = outarg;
	  
      };
      
      for(auto& arg : argvec)
      {
	bool success;
	auto str = arg.to_string(&success);
	if(!success)
	{
	  lg.Error("couldn't get string whil trying to print arg");
	  break;
	}
	
	lg.Trace("arg: " + str);
	
      }
      
      
      return argvec;
      

    }
      
    
    
}
