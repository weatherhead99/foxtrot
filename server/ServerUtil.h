#pragma once
#include "foxtrot.grpc.pb.h"
#include <rttr/type>


namespace foxtrot
{
    
    
    template <typename replT, typename errT> void set_repl_err(replT& repl, errT& err, error_types etype)
    {
        auto errstat = repl.mutable_err();
        errstat->set_msg(err.what());
        errstat->set_tp(etype);
    };
    
    
    template <typename retT, typename reqT> retT init_chunk(reqT& req)
    {
      retT out;
      out.set_msgid(req.msgid());
      out.set_devid(req.devid());
      out.set_capname(req.capname());
      
      return out;
      
    }
    
    rttr::variant get_arg(const capability_argument& arg, bool& success);
    
    
    
}
