#pragma once
#include "CommunicationProtocol.h"
#include <string>
#include <vector>

namespace foxtrot
{
    namespace protocols
    {
        class i2c : public CommunicationProtocol
        {
        public:
            i2c(const parameterset* const instance_parameters);
            virtual ~i2c() override;
            virtual void Init(const parameterset *const class_parameters) override;
            std::vector<unsigned char> read_block_data(int cmd, int len);
            void write_block_data(int cmd, const std::vector<unsigned char>& data);
	    
	    
        private:
            std::string _devnode;
            unsigned _address;
            
            int _fd;
        };
        
        
    }
    
}
