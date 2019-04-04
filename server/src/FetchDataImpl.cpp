#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>

#include <foxtrot/server/Device.h>
#include <foxtrot/server/ServerUtil.h>

#include "FetchDataImpl.h"

foxtrot::FetchDataLogic::FetchDataLogic(foxtrot::DeviceHarness& harness)
: _harness(harness), _lg("FetchDataLogic")
{
}

bool foxtrot::FetchDataLogic::initial_request(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
{
    _lg.Debug("processing fetch data request" );
    
    foxtrot::Device* dev;
    repl = init_chunk<foxtrot::datachunk>(req);
    
    
    try
    {
     dev = _harness.GetDevice(req.devid());   
    }
    catch(std::out_of_range& err)
    {
      foxtrot_server_specific_error("invalid device", repl,respond, _lg,tag,error_types::out_of_range);
        return true;
    }
    
    auto devtp = rttr::type::get(*dev);
    auto meth = devtp.get_method(req.capname().c_str());
    
    if(!meth)
    {
      auto msg = "no matching method found for capability: " + req.capname();
      foxtrot_server_specific_error(msg,repl,respond,_lg,tag,error_types::out_of_range);
      return true;
    }
    
    auto streamdatameta = meth.get_metadata("streamdata");
    if(!streamdatameta.is_valid() || !streamdatameta.to_bool()) 
    {
      auto msg = "tried to call a method that doesn't support streaming";
      foxtrot_server_specific_error(msg,repl,respond,_lg,tag);
      return true;
    }
    
    rttr::variant retval;
    
    std::vector<rttr::variant> args;
    
    try{
      args = get_callargs(meth,req,repl);
    }
    catch(int& i)
    {
      foxtrot_server_specific_error("couldn't get callargs", repl, respond, _lg,tag);
      return true;
    }
    
    
    try{
      std::vector<rttr::argument> callargs(args.begin(), args.end());
      
      auto lock = _harness.lock_device_contentious(req.devid(),req.contention_timeout());
      retval = meth.invoke_variadic(*dev,callargs);
    }
    catch(...)
    {
      foxtrot_rpc_error_handling(std::current_exception(),repl,respond,_lg,tag);
      return true;
    };
    
#ifdef NEW_RTTR_API
    if(!retval.is_sequential_container())
#else
    if(!retval.is_array())
#endif
    {
      foxtrot_server_specific_error("return type is not an array type", repl, respond,_lg,tag);
      return true;
    }
    
    foxtrot::byte_data_types dt;
    _data = foxtrot::byte_view_data(retval,_byte_size,dt);
    
    if(_data == nullptr )
    {
      foxtrot_server_specific_error("can't convert return type to vector", repl, respond, _lg,tag);
      return true;
    }
    
    repl.set_dtp(dt);
    _csize = req.chunksize();

    _num_full_chunks = (_byte_size / _csize);
    _extra_chunk = _byte_size % _csize ? true : false;
    
    _lg.Trace("num full chunks: " + std::to_string(_num_full_chunks));
    _lg.Trace("extra chunk: " + std::to_string(_extra_chunk));
    
    _currval = _data.get();
    
    return false;
};


bool foxtrot::FetchDataLogic::HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag)
{
    if(_thischunk == 0)
    {
        if(initial_request(req,repl,respond,tag))
        {
            //if that returned true, there's an error which has already been dispatched
	  //TODO: consistent respond finish logic
            return true;
        };
    }
    
    _lg.Trace("thischunk: " + std::to_string(_thischunk));
    _lg.Trace("num_full_chunks: " + std::to_string(_num_full_chunks));
    
    
    if(_thischunk < _num_full_chunks)
    {
        auto outdat = repl.mutable_data();
        outdat->assign(_currval, _currval+_csize);
        _currval += _csize;
        _lg.Trace("writing chunk to wire: " + std::to_string(_thischunk));
        respond.Write(repl,tag);
        _thischunk++;
        return false;
        
    }
        
    if(_extra_chunk)
    {
      _lg.Debug("extra chunk writing...");
      auto outdat = repl.mutable_data();
      _lg.Trace("WARNING, possible seGFAULT!");
      outdat->assign(_currval,_data.get() + _byte_size);
      respond.Write(repl,tag); 
      _lg.Trace("all chunks written...");
      _extra_chunk = false;
      _thischunk++;
      return false;
    }
    
    _lg.Trace("finishing responder...");
    respond.Finish(grpc::Status::OK,tag);
    _thischunk = 0;
    return true;
    
    
        
}
