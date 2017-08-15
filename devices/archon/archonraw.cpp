#include "archonraw.h"



std::string foxtrot::devices::archonraw::rawcmd(const std::string& request)
{
   std::unique_lock<std::mutex> lck(_cmdmut);
   _specproto->write(request);
   
   auto repl = _specproto->read_until_endl();
   
   lck.unlock();
   
   return repl;
   
    
};


RTTR_REGISTRATION
{
    using namespace rttr;
    using foxtrot::devices::archonraw;
    
    
    registration::class_<archonraw>("foxtrot::devices::archonraw")
    .method("rawcmd",&archonraw::rawcmd)
    (
        parameter_names("request")
        )
    ;
    
    
}
