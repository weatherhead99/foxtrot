#pragma once
#include <map>

#include <grpc++/grpc++.h>

#include <foxtrot/ft_capability.grpc.pb.h>
#include <foxtrot/Logging.h>

#include <foxtrot/server/DeviceHarness.h>
#include "Logic_defs.hh"

namespace foxtrot
{
    class HandlerTag;
    
    struct FetchDataLogic : public Serverlogic_defs<&capability::AsyncService::RequestFetchData>
    {
        FetchDataLogic(std::shared_ptr<DeviceHarness> harness);

        bool HandleRequest(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);

	
    private:
        bool initial_request(reqtp& req, repltp& repl, respondertp& respond, HandlerTag* tag);
        std::shared_ptr<DeviceHarness> _harness;
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
    
    
    
}
