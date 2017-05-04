#pragma once
#include "CommunicationProtocol.h"
#include <string>
#include <vector>
#include "Logging.h"

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
            std::vector<unsigned char> read_block_data(unsigned char cmd, int len);
//             void write_block_data(int cmd, const std::vector<unsigned char>& data);
	    
	    void write_byte_data(unsigned char reg, unsigned char val);
	    
	    
        private:
            foxtrot::Logging _lg;
            std::string _devnode;
            unsigned _address;
            
            int _fd;
        };
        
        
    }
    
}
