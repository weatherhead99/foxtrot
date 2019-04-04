#include "HandlerBase.h"

template<typename T> foxtrot::HandlerBase<T>::HandlerBase(exptserve::AsyncService* service, grpc::ServerCompletionQueue* cq)
: _service(service), _cq(cq), _responder(&_ctxt), _status(status::CREATE)
{
    
}
