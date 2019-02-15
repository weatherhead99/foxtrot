#pragma once

#include "Device.h"
#include "Logging.h"

#include <rapidxml.hpp>
#include <rttr/registration>
#include <sstream>
#include "curlRequest.h"

using foxtrot::protocols::CurlRequest;

namespace foxtrot{
    namespace devices {
 
        class webswitch_plus : public Device 
        {
          RTTR_ENABLE(Device);
        public:
            using protoT = std::shared_ptr<CurlRequest>;
            
            webswitch_plus(protoT& proto, const std::string& url);
            webswitch_plus(protoT& proto, const foxtrot::parameterset& params);
            const std::string getDeviceTypeName() const override;
            virtual ~webswitch_plus();
            
            void SetRelay(unsigned short chan, bool onoff);
            bool GetRelay( unsigned short chan);
            
        private:
            std::string _url;
            rapidxml::xml_document<> _doc;
            std::ostringstream _oss;
            Logging _lg;
        };
        
    }
    
}
