#pragma once
#include "foxtrot.grpc.pb.h"

namespace foxtrot
{
    
    
    template <typename replT, typename errT> void set_repl_err(replT& repl, errT& err, error_types etype)
    {
        auto errstat = repl.mutable_err();
        errstat->set_msg(err.what());
        errstat->set_tp(etype);
    };
    
    
    
    
    
}
