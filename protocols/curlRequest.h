#pragma once
#include "CommunicationProtocol.h"
#include "Logging.h"
#include <sstream>

using namespace foxtrot;
using std::string;


namespace detail  {
    
    
}
        
namespace foxtrot {
    namespace protocols {
        
        class CurlRequest : public CommunicationProtocol
        {
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
