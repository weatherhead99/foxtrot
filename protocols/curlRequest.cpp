#include "curlRequest.h"
#include <curl/curl.h>
#include <curl/easy.h>


using namespace foxtrot;
using namespace foxtrot::protocols;

int CurlRequest::_nCurlInstances = 0;


CurlRequest::CurlRequest() 
: CommunicationProtocol(nullptr)
{
    //WARNING: watch out for thread safety issues!
    if(_nCurlInstances == 0)
    {
        auto ret = curl_global_init(CURL_GLOBAL_ALL);
    }
    
    //increment the number of instances of this class around
    _nCurlInstances++;
}

foxtrot::protocols::CurlRequest::~CurlRequest()
{
    _nCurlInstances--;
    
}


void CurlRequest::Init(const parameterset *const)
{
}


string CurlRequest::blocking_get_request(const string& path)
{
}
