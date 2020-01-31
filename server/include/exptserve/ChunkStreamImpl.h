#pragma once
#include <memory>

#include <grpc++/grpc++.h>

#include <foxtrot/Logging.h>
#include <foxtrot/foxtrot.grpc.pb.h>

#include "HandlerBase.h"
#include "Logic_defs.hh"

namespace foxtrot
{
 struct ChunkStreamLogic : public
 Serverlogic_defs<&exptserve::AsyncService::RequestFetchData>
 {

   ChunkStreamLogic();
   
   bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, void* tag);
   
   unsigned _bytesize;
   unsigned _chunksize;
   foxtrot::byte_data_types _dt;
   
   
 private:
   foxtrot::Logging _lg;
   std::unique_ptr<unsigned char[]> _data;
   unsigned _thischunk = 0;
   unsigned _num_chunks;
   bool _extra_chunk;
   bool _allchunksdone = false;
   unsigned char* _currval;
   
 };

  
}
