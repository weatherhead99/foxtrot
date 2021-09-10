#include "ChunkStreamImpl.h"

//TODO: set request status properly
foxtrot::ChunkStreamLogic::ChunkStreamLogic()
: _lg("ChunkStreamLogic")
{

}


bool foxtrot::ChunkStreamLogic::HandleRequest(foxtrot::ChunkStreamLogic::reqtp& req, foxtrot::ChunkStreamLogic::repltp& repl, foxtrot::ChunkStreamLogic::respondertp& respond, void* tag)
{
  //TODO: how to get these variables initialized properly....
  if(_thischunk == 0)
  {
  //work out how many chunks
    _num_chunks  = _bytesize / _chunksize;
    _extra_chunk = _bytesize % _chunksize ? true: false;
    
    _lg.Trace("num chunks: " + std::to_string(_num_chunks));
    _currval = _data.get();
    
    repl.set_dtp(_dt);
  }
  
    
  if(_thischunk < _num_chunks)
  {
    auto outdat = repl.mutable_data();
    outdat->assign(_currval,_currval + _chunksize);
    
    _lg.Trace("writing chunk to wire: " + std::to_string(_thischunk));
    respond.Write(repl,tag);
    
    _thischunk++;
    _currval += _chunksize;
    //NOTE: request is NOT finished!
    return false;
    
  }
    
  if(_extra_chunk)
  {
    _lg.Debug("writing extra chunk");
    auto outdat = repl.mutable_data();
    
    _lg.Trace("segFAULT ahoy!");
    outdat->assign(_currval,_data.get() + _bytesize);
    respond.Write(repl,tag);
    
    _allchunksdone = true;
    return false;
  }
  
  if(_allchunksdone)
  {
    respond.Finish(grpc::Status::OK,tag);
    return true;
  }

  //WARNING: not sure about this
  return true;

}
