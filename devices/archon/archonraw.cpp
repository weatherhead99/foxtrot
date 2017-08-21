#include "archonraw.h"
#include <chrono>
#include <thread>


devices::archonraw::archonraw(std::shared_ptr<foxtrot::protocols::simpleTCP> proto)
: foxtrot::devices::archon(proto)
{

}


std::string foxtrot::devices::archonraw::rawcmd(const std::string& request)
{
   
    std::unique_lock<std::mutex> lck(_cmdmut);
   _specproto->write(request);
    
   std::string out;
   
   
    if(request.substr(1,5) == "FETCH")
    {
        //binary response
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        while(_specproto->bytes_available() >= 1028)
        {
            unsigned actlen;
            out += _specproto->read(1028,&actlen);
            _lg.Trace("read: " + std::to_string(actlen));
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        };
        
        return out;
    }
    
    
   
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
