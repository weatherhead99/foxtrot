#pragma once
#include "HandlerBase.h"
#include "foxtrot.grpc.pb.h"
#include <grpc++/grpc++.h>
#include "Logging.h"

#include <memory>

namespace foxtrot
{
 struct ChunkStreamLogic
 {
   typedef chunk_request reqtp;
   typedef datachunk repltp;
   typedef grpc::ServerAsyncWriter<datachunk> respondertp;
   
   //NOTE: will not be used, request will always start in PROCESS mode
   constexpr static auto requestfunptr = &exptserve::AsyncService::RequestFetchData;
   const static bool newcall = false;
   
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
  
 typedef HandlerBase<ChunkStreamLogic> ChunkStreamImpl;
  
}