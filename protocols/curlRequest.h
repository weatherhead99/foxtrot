#pragma once
#include "CommunicationProtocol.h"

using namespace foxtrot;
using std::string;

        
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
            static int _nCurlInstances;
            
        };
        
    }
}
