#include "FetchDataImpl.h"
#include "Device.h"

#include "ServerUtil.h"

foxtrot::FetchDataLogic::FetchDataLogic(foxtrot::DeviceHarness& harness)
: _harness(harness), _lg("FetchDataLogic")
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
    auto meth = devtp.get_method(req.capname().c_str());
    
    if(!meth)
    {
      _lg.Error("no matching method found for capability: " + req.capname());
      auto repl = init_chunk<foxtrot::datachunk>(req);
      auto errstat = repl.mutable_err();
      errstat->set_msg("no matching method found");
      errstat->set_tp(error_types::out_of_range);
      writer.Write(repl);
      
      return;
    }
    
    rttr::variant retval;
    
    auto args = req.args();
    
    
    
    
    
    
}
