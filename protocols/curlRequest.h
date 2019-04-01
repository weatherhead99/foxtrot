#pragma once
#include "CommunicationProtocol.h"
#include "Logging.h"
#include <sstream>
#include <map>

using namespace foxtrot;
using std::string;
using std::map;
using std::vector;

class curl_slist;

using slistuptr = std::unique_ptr<curl_slist, void(*)(curl_slist*)>;

namespace detail  {
    size_t write_cback(char* ptr, size_t size, size_t nmemb, void* userdata);
    
}
        
namespace foxtrot {
    namespace protocols {
        
        class CurlRequest : public CommunicationProtocol
        {
        friend size_t detail::write_cback(char*, size_t, size_t, void*);
        public:
            CurlRequest();
            virtual ~CurlRequest();
            
            void Init(const parameterset* const ) override;
            std::string blocking_get_request(const string& path,
                                             const vector<string>* header = nullptr);
            std::string blocking_post_request(const string& path,
                                              const string& body,
                                              const vector<string>* header = nullptr); 
            
            unsigned long get_last_http_response_code();
            
        private:
            void curl_checkerror(int code);
            
            void curl_common_performreq();
            
            slistuptr set_curl_header(const vector<string>& headerfields);
            
            std::ostringstream& getdatabuilder();
            
            
            std::ostringstream thisreq_builder;
            static int _nCurlInstances;
            void* _curlinstance;
            Logging _lg;
        };
        
    }
}
