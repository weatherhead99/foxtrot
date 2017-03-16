#include "FetchDataImpl.h"
#include "Device.h"

#include "ServerUtil.h"

foxtrot::FetchDataLogic::FetchDataLogic(foxtrot::DeviceHarness& harness)
: _harness(harness)
{
}




void foxtrot::FetchDataLogic::HandleRequest(reqtp& req, repltp& writer)
{
    std::cout << "processing fetch data request" << std::endl;
    
    
    foxtrot::Device* dev;
    
    try
    {
     dev = _harness.GetDevice(req.devid());   
    }
    catch(std::out_of_range& err)
    {
        auto repl = init_chunk<foxtrot::datachunk>(req);
        
        auto errstat = repl.mutable_err();
        errstat->set_msg(err.what());
        errstat->set_tp(error_types::out_of_range);
        writer.Write(repl);
        return;
        
    }
    
    auto devtp = rttr::type::get(*dev);
    
    auto prop = devtp.get_property(req.capname().c_str());
    auto meth = devtp.get_method(req.capname().c_str());
    
    
    
    
    
}
