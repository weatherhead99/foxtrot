#include "FetchDataImpl.h"
#include "Device.h"

#include "ServerUtil.h"
#include "DeviceError.h"
#include "ProtocolError.h"

foxtrot::FetchDataLogic::FetchDataLogic(foxtrot::DeviceHarness& harness)
: _harness(harness), _lg("FetchDataLogic")
{
}




void foxtrot::FetchDataLogic::HandleRequest(reqtp& req, repltp& writer)
{
    std::cout << "processing fetch data request" << std::endl;
    
    foxtrot::Device* dev;
    auto repl = init_chunk<foxtrot::datachunk>(req);
    
    try
    {
     dev = _harness.GetDevice(req.devid());   
    }
    catch(std::out_of_range& err)
    {
        set_repl_err(repl,err,error_types::out_of_range);
        writer.Write(repl);
        return;
        
    }
    
    auto devtp = rttr::type::get(*dev);
    auto meth = devtp.get_method(req.capname().c_str());
    
    if(!meth)
    {
      _lg.Error("no matching method found for capability: " + req.capname());
      auto errstat = repl.mutable_err();
      errstat->set_msg("no matching method found");
      errstat->set_tp(error_types::out_of_range);
      writer.Write(repl);
      
      return;
    }
    
    auto streamdatameta = meth.get_metadata("streamdata");
    if(!streamdatameta.is_valid())
    {
      auto msg = "tried to call a method that doesn't support streaming";
      set_repl_err_msg(repl,msg,error_types::out_of_range);
    }
    
    rttr::variant retval;
    
    std::vector<rttr::argument> callargs;
    
    try{
      callargs = get_callargs(meth,req,repl);
    }
    catch(int& i)
    {
      return;
    }
    
    auto& mut = _harness.GetMutex(req.devid());
    std::lock_guard<std::mutex> lock(mut);
    
    try{
    retval = meth.invoke_variadic(*dev,callargs);
    }
    catch(class foxtrot::DeviceError& err)
    {
      _lg.Error("caught device error: " + std::string(err.what()));
      set_repl_err(repl,err,error_types::DeviceError);
      writer.Write(repl);
      return;
    }
    catch(class foxtrot::ProtocolError& err)
    {
      _lg.Error("caught protocol error: " + std::string(err.what()));
      set_repl_err(repl,err,error_types::ProtocolError);
      writer.Write(repl);
      return;
    }
    catch(std::exception& err)
    {
      _lg.Error("caught general error: " + std::string(err.what()));
      set_repl_err(repl,err,error_types::ProtocolError);
      writer.Write(repl);
      return;
    }
    
    if(!retval.is_array())
    {
      _lg.Error("return type is not an array type");
      set_repl_err_msg(repl,"return type is not an array type",error_types::Error); 
    }
    
    auto arrtp = rttr::type::get<std::vector<unsigned char>>();
    if(!retval.can_convert(arrtp))
    {
      _lg.Error("can't convert return type to unsigned char vector");
    }
    
    auto data = retval.convert<std::vector<unsigned char>>();
    
    auto csize = req.chunksize();
    
    unsigned num_chunks = data.size() / csize;
    bool extra_chunk = data.size() % csize ? true : false;
    
    auto currval = data.begin();
    for(int i =0 ; i < num_chunks; i++)
    {
     repl = init_chunk<foxtrot::datachunk>(req);
     auto outdat = repl.mutable_data();
     outdat->assign(currval, currval + csize);
     currval += csize;
     writer.Write(repl);
    }
    
    if(extra_chunk)
    {
      repl = init_chunk<foxtrot::datachunk>(req);
      auto outdat = repl.mutable_data();
      outdat->assign(currval,data.end());
      writer.Write(repl);
    }
    
}
