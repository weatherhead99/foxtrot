#pragma once
#include <sstream>
#include <map>
#include <foxtrot/Logging.h>
#include <foxtrot/protocols/CommunicationProtocol.h>

using namespace foxtrot;
using std::string;
using std::map;
using std::vector;

class curl_slist;

using slistuptr = std::unique_ptr<curl_slist, void ( * ) ( curl_slist* ) >;


namespace foxtrot
{

namespace detail
{
size_t write_cback ( char* ptr, size_t size, size_t nmemb, void* userdata );

}

  
namespace protocols
{

class CurlRequest : public CommunicationProtocol
{
  friend size_t foxtrot::detail::write_cback ( char*, size_t, size_t, void* );
public:
    CurlRequest();
    virtual ~CurlRequest();

  void Init ( const parameterset* const, bool open_immediate=true ) override;
    std::string blocking_get_request ( const string& path,
                                       const vector<string>* header = nullptr );
    std::string blocking_post_request ( const string& path,
                                        const string& body,
                                        const vector<string>* header = nullptr );

    unsigned long get_last_http_response_code(void* curlinstance);
  unsigned long get_last_http_response_code();
  
    std::string get_redirect_url(void* curlinstance);

private:
    void curl_checkerror ( int code );
  unsigned int last_code = 0;
    void curl_common_performreq(void* curlinstance);

  [[nodiscard]] slistuptr set_curl_header ( const vector<string>& headerfields, void* curlinstance );

    std::ostringstream& getdatabuilder();


    std::ostringstream thisreq_builder;
  static std::atomic<int> _nCurlInstances;
  //    void* _curlinstance;
    Logging _lg;


  template<typename CurlInstance>
  slistuptr common_curl_setup(CurlInstance inst, const string& path, const vector<string>* header);
  
};

}
}
