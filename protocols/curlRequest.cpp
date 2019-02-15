#include "curlRequest.h"
#include "ProtocolError.h"
#include <curl/curl.h>
#include <curl/easy.h>


using namespace foxtrot;
using namespace foxtrot::protocols;

int CurlRequest::_nCurlInstances = 0;


size_t detail::write_cback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
        auto* req = reinterpret_cast<CurlRequest*>(userdata);
        std::string stringdat(ptr,nmemb);
        req->getdatabuilder() << stringdat;

      return nmemb;
        
}


CurlRequest::CurlRequest() 
: CommunicationProtocol(nullptr), _lg("CurlRequest")
{
    //WARNING: watch out for thread safety issues!
    if(_nCurlInstances == 0)
    {
        auto ret = curl_global_init(CURL_GLOBAL_ALL);
        if(ret != CURLE_OK)
        {
            auto strer = curl_easy_strerror(ret);
            throw foxtrot::ProtocolError(strer, _lg);
        }
    }
    
    //increment the number of instances of this class around
    _nCurlInstances++;
    
    _curlinstance = curl_easy_init();
    if(!_nCurlInstances)
    {
        throw foxtrot::ProtocolError("couldn't initialize libcurl instance", _lg);
    }
    
    
}

foxtrot::protocols::CurlRequest::~CurlRequest()
{
    curl_easy_cleanup(_curlinstance);
    
    _nCurlInstances--;
    if(_nCurlInstances == 0)
    {
        curl_global_cleanup();
    }
    
}


void CurlRequest::Init(const parameterset *const)
{
}


string CurlRequest::blocking_get_request(const string& path)
{
    thisreq_builder.str("");
    curl_checkerror(curl_easy_setopt(_curlinstance, CURLOPT_URL, path.c_str()));
//     
    _lg.strm(sl::debug) << "setting write function";
    //NOTE: the + here is drastically important if using a lambda, for ..... reasons.. 
    // (to do with C variadics)
    curl_checkerror(curl_easy_setopt(_curlinstance,CURLOPT_WRITEFUNCTION, detail::write_cback));
    _lg.strm(sl::debug) << "setting write data";
    curl_checkerror(curl_easy_setopt(_curlinstance,CURLOPT_WRITEDATA,
        reinterpret_cast<void*>(this)));
    
    _lg.strm(sl::debug) << "performing request";
    curl_checkerror(curl_easy_perform(_curlinstance));

    _lg.strm(sl::debug) << "request done";
    return thisreq_builder.str();
}

void foxtrot::protocols::CurlRequest::curl_checkerror(int code)
{
    CURLcode ccode = (CURLcode) code;
    
    if(ccode != CURLE_OK)
    {
        auto strer = curl_easy_strerror(ccode);
        throw foxtrot::ProtocolError(strer, _lg);
    }
    
}


std::ostringstream & foxtrot::protocols::CurlRequest::getdatabuilder()
{
    return thisreq_builder;
}

