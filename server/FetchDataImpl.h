#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include "HandlerBase.h"
#include "DeviceHarness.h"
#include "Logging.h"

#include <map>

namespace foxtrot
{
    struct FetchDataLogic
    {
        typedef chunk_request reqtp;
        typedef datachunk repltp; 
	typedef grpc::ServerAsyncWriter<datachunk> respondertp;
	  
        constexpr static auto requestfunptr = &exptserve::AsyncService::RequestFetchData;
        const static bool newcall = true;
	
        FetchDataLogic(DeviceHarness& harness);
        
	
        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
        
        
        
	
    private:
        bool initial_request(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
        DeviceHarness& _harness;
	foxtrot::Logging _lg;
    
    unsigned _thischunk = 0;
      unsigned _num_full_chunks;  
      bool _extra_chunk;
      bool _alldone = false;
      unsigned _csize;
      unsigned _byte_size;
      std::unique_ptr<unsigned char[]> _data;
      unsigned char* _currval;
    };
    
    
    typedef HandlerBase<FetchDataLogic> FetchDataImpl;

    
    
}
