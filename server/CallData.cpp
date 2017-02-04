#include "CallData.h"
#include <grpc++/grpc++.h>

#include <iostream>
using std::cout;
using std::endl;

foxtrot::CallData::CallData(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq)
: _service(service), _cq(cq), _responder(&_ctxt), _status(status::CREATE)
{
    cout << "calldata constructing " <<endl;
   Proceed(); 
 
}


void foxtrot::CallData::Proceed()
{
    if(_status == status::CREATE)
    {
        cout <<"status to process.." << endl;
     _status = status::PROCESS;   
     
     _service->RequestInvokeCapability(&_ctxt, &_req, &_responder, _cq, _cq,this);
     cout << "requested invoke" << endl;
    }
    else if(_status == status::PROCESS)
    {
      new CallData(_service, _cq);
        
    //actual processing goes here!
    _status = status::FINISH;
    _responder.Finish(_reply,grpc::Status::CANCELLED,this);
    }
    else
    {
        cout << "request finished, deleting.." << endl;
        GPR_CODEGEN_ASSERT(_status == status::FINISH);
        delete this;
    }
    
    
    
}

