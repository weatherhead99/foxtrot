#pragma once
#include <grpc++/grpc++.h>
#include "foxtrot.pb.h"
#include "foxtrot.grpc.pb.h"

namespace foxtrot{

class CallData
{
public:
    CallData(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq);
    
    void Proceed();
    
private:
    exptserve::AsyncService* _service;
    grpc::ServerCompletionQueue* _cq;
    grpc::ServerContext _ctxt;
    
    capability_request _req;
    capability_response _reply;
    
    grpc::ServerAsyncResponseWriter<capability_response> _responder;
    
    enum class status {CREATE, PROCESS, FINISH};
    status _status;
    
};


}
