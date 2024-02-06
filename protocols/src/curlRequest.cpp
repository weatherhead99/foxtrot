#include <curl/curl.h>
#include <curl/easy.h>

#include <iostream>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/protocols/curlRequest.h>


using namespace foxtrot;
using namespace foxtrot::protocols;

std::atomic<int> CurlRequest::_nCurlInstances = 0;



size_t foxtrot::detail::write_cback(char* ptr, size_t size, size_t nmemb, void* userdata)
{

  std::cout << "in write_cback" << std::endl;
        auto* req = reinterpret_cast<CurlRequest*>(userdata);
        std::string stringdat(ptr,nmemb);
        req->getdatabuilder() << stringdat;

	std::cout << "write_cback done" << std::endl;
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
    
    
}

foxtrot::protocols::CurlRequest::~CurlRequest()
{
    
    _nCurlInstances--;
    if(_nCurlInstances == 0)
    {
        curl_global_cleanup();
    }
    
}



template <typename CurlInstance>
slistuptr CurlRequest::common_curl_setup(CurlInstance inst, const string& path, const vector<string>* header)
{
  thisreq_builder.str("");
  curl_checkerror(curl_easy_setopt(inst, CURLOPT_URL, path.c_str()));

  slistuptr headerptr(nullptr, curl_slist_free_all);

  if(header)
    {
      _lg.Debug("header present");
      headerptr = set_curl_header(*header, inst);
    }

  return std::move(headerptr);

}
		  



void CurlRequest::Init(const parameterset *const)
{

}




string CurlRequest::blocking_get_request(const string& path,
                                         const vector<string>* header
)
{

  std::unique_ptr<CURL, void(*)(CURL*)> inst(curl_easy_init(), curl_easy_cleanup);
  


  auto hdrs = common_curl_setup(inst.get(), path, header);  
  curl_common_performreq(inst.get());
 
  
  return thisreq_builder.str();
}

string CurlRequest::blocking_post_request(const string& path,
                                          const string& body,
                                          const vector<string>* header)
{

  std::unique_ptr<CURL, void(*)(CURL*)> inst(curl_easy_init(), curl_easy_cleanup);

  auto hdrs = common_curl_setup(inst.get(), path, header);
    curl_checkerror(curl_easy_setopt(inst.get(), CURLOPT_POSTFIELDS, body.c_str()));

    curl_common_performreq(inst.get());

    
    return thisreq_builder.str();
};


void foxtrot::protocols::CurlRequest::curl_checkerror(int code)
{
    CURLcode ccode = (CURLcode) code;
    
    if(ccode != CURLE_OK)
    {
        auto strer = curl_easy_strerror(ccode);
        throw foxtrot::ProtocolError(strer, _lg);
    }
    
}

unsigned long foxtrot::protocols::CurlRequest::get_last_http_response_code()
{
  return last_code;
}


unsigned long foxtrot::protocols::CurlRequest::get_last_http_response_code(void* inst)
{
    unsigned long code;
    curl_checkerror(curl_easy_getinfo(inst,CURLINFO_RESPONSE_CODE,&code));
    return code;
    
}

std::string foxtrot::protocols::CurlRequest::get_redirect_url(void* curlinstance)
{
    char* url;
    curl_checkerror(curl_easy_getinfo(curlinstance, CURLINFO_REDIRECT_URL, &url));
    return std::string(url);
}




void foxtrot::protocols::CurlRequest::curl_common_performreq(void* curlinstance)
{
    _lg.strm(sl::debug) << "setting write function";
    //NOTE: the + here is drastically important if using a lambda, for ..... reasons.. 
    // (to do with C variadics)
        curl_checkerror(curl_easy_setopt(curlinstance,CURLOPT_WRITEFUNCTION, detail::write_cback));
    _lg.strm(sl::debug) << "setting write data";
    curl_checkerror(curl_easy_setopt(curlinstance,CURLOPT_WRITEDATA,
      reinterpret_cast<void*>(this)));
    
    _lg.strm(sl::debug) << "performing request";

    
    auto ret = curl_easy_perform(curlinstance);

    _lg.strm(sl::debug) << "returned from easy_perform, ret is : " << (int) ret;
    curl_checkerror(ret);

    
    _lg.strm(sl::debug) << "request done";

    last_code = get_last_http_response_code(curlinstance);
    
    
};

slistuptr foxtrot::protocols::CurlRequest::set_curl_header(const vector<std::string>& headerfields, void* curlinst)
{
    std::unique_ptr<curl_slist, void(*)(curl_slist*)> list(nullptr, curl_slist_free_all);
    
    curl_slist* raw_list = nullptr;
    
    for(auto& item : headerfields)
    {
        raw_list = curl_slist_append(raw_list, item.c_str());
    }
    list.reset(raw_list);
    
    curl_checkerror(curl_easy_setopt(curlinst, CURLOPT_HTTPHEADER, list.get()));
    
    return list;
}



std::ostringstream & foxtrot::protocols::CurlRequest::getdatabuilder()
{
    return thisreq_builder;
}

