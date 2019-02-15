#pragma once
#include "CommunicationProtocol.h"
#include "Logging.h"
#include <sstream>

using namespace foxtrot;
using std::string;


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
            std::string blocking_get_request(const string& path);
            
        private:
            void curl_checkerror(int code);
            
            std::ostringstream& getdatabuilder();
            
            
            std::ostringstream thisreq_builder;
            static int _nCurlInstances;
            void* _curlinstance;
            Logging _lg;
        };
        
    }
}
