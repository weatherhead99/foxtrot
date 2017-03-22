#include "FetchDataImpl.h"
#include "Device.h"

#include "ServerUtil.h"
#include "DeviceError.h"
#include "ProtocolError.h"

foxtrot::FetchDataLogic::FetchDataLogic(foxtrot::DeviceHarness& harness)
: _harness(harness), _lg("FetchDataLogic")
{
}




bool foxtrot::FetchDataLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, void* tag)
{
    std::cout << "processing fetch data request" << std::endl;
    
    foxtrot::Device* dev;
    repl = init_chunk<foxtrot::datachunk>(req);
    repl.set_dataid(req.dataid());
    
    try
    {
     dev = _harness.GetDevice(req.devid());   
    }
    catch(std::out_of_range& err)
    {
        set_repl_err(repl,err,error_types::out_of_range);
        respond.Write(repl,tag);
// 	respond.Finish(grpc::Status::OK,tag);
        return true;
        
    }
    
    auto devtp = rttr::type::get(*dev);
    auto meth = devtp.get_method(req.capname().c_str());
    
    if(!meth)
    {
      _lg.Error("no matching method found for capability: " + req.capname());
      auto errstat = repl.mutable_err();
      errstat->set_msg("no matching method found");
      errstat->set_tp(error_types::out_of_range);
      respond.Write(repl,tag);
//       respond.Finish(grpc::Status::OK,tag);
      
      return true;
    }
    
    auto streamdatameta = meth.get_metadata("streamdata");
    if(!streamdatameta.is_valid())
    {
      auto msg = "tried to call a method that doesn't support streaming";
      set_repl_err_msg(repl,msg,error_types::out_of_range);
      respond.Write(repl,tag);
      respond.Finish(grpc::Status::OK,tag);
      return true;
    }
    
      if(!streamdatameta.to_bool())
      {
	auto msg = "tried to call a method that doesn't support streaming";
      set_repl_err_msg(repl,msg,error_types::out_of_range);
      respond.Write(repl,tag);
      respond.Finish(grpc::Status::OK,tag);
      return true;
      };
      
    
    rttr::variant retval;
    
    std::vector<rttr::variant> args;
    
    try{
      args = get_callargs(meth,req,repl);
    }
    catch(int& i)
    {
      set_repl_err_msg(repl,"couldn't get callargs",error_types::Error);
      respond.Write(repl,tag);
      return true;
    }
    
    auto& mut = _harness.GetMutex(req.devid());
    std::lock_guard<std::mutex> lock(mut);
    
    try{
      std::vector<rttr::argument> callargs(args.begin(), args.end());
    retval = meth.invoke_variadic(*dev,callargs);
    }
    catch(class foxtrot::DeviceError& err)
    {
      _lg.Error("caught device error: " + std::string(err.what()));
      set_repl_err(repl,err,error_types::DeviceError);
      respond.Write(repl,tag);
//       respond.Finish(grpc::Status::OK,tag);
      return true;
    }
    catch(class foxtrot::ProtocolError& err)
    {
      _lg.Error("caught protocol error: " + std::string(err.what()));
      set_repl_err(repl,err,error_types::ProtocolError);
      respond.Write(repl,tag);
//       respond.Finish(grpc::Status::OK,tag);
      return true;
    }
    catch(std::exception& err)
    {
      _lg.Error("caught general error: " + std::string(err.what()));
      set_repl_err(repl,err,error_types::ProtocolError);
      respond.Write(repl,tag);
//       respond.Finish(grpc::Status::OK,tag);
      return true;
    }
    
    if(!retval.is_array())
    {
      _lg.Error("return type is not an array type");
      set_repl_err_msg(repl,"return type is not an array type",error_types::Error); 
      respond.Write(repl,tag);
//       respond.Finish(grpc::Status::OK,tag);
      return true;
    }
    
    auto arrtp = rttr::type::get<std::vector<unsigned char>>();
    if(!retval.can_convert(arrtp))
    {
      _lg.Error("can't convert return type to unsigned char vector");
      set_repl_err_msg(repl,"can't convert return type to uchar vector",error_types::Error);
      respond.Write(repl,tag);
//       respond.Finish(grpc::Status::OK,tag);
    }
    
    
    //get the cq, we'll need it later
    _lg.Debug("getting completion queue");
    auto cq = reinterpret_cast<FetchDataImpl*>(tag)->GetCQ();
    
    
    auto data = retval.convert<std::vector<unsigned char>>();
    
    auto csize = req.chunksize();
    
    unsigned num_chunks = data.size() / csize;
    bool extra_chunk = data.size() % csize ? true : false;
    
    auto currval = data.begin();
    //
    long unsigned atag = reinterpret_cast<long unsigned>(tag) +1;
    bool ok;
    for(int i =0 ; i < num_chunks; i++)
    {
     auto outdat = repl.mutable_data();
     outdat->assign(currval, currval + csize);
     currval += csize;
     respond.Write(repl,reinterpret_cast<void*>(atag++));
     cq->Next((void**) &atag,&ok);
     if(!ok)
     {
      _lg.Error("completion queue next failed!"); 
     }
    }
    
    if(extra_chunk)
    {
      repl = init_chunk<foxtrot::datachunk>(req);
      auto outdat = repl.mutable_data();
      outdat->assign(currval,data.end());
      respond.Write(repl,reinterpret_cast<void*>(atag++));
      cq->Next( (void**) &atag,&ok);
      if(!ok)
      {
	_lg.Error("completion queue next failed!");
      }
    }
    
    respond.Finish(grpc::Status::CANCELLED,reinterpret_cast<void*>(atag++));
    return true;
    
}
