#include "i2c.h"
#include "ProtocolUtilities.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <sys/ioctl.h>

foxtrot::protocols::i2c::i2c(const parameterset *const instance_parameters)
: foxtrot::CommunicationProtocol(instance_parameters), _lg("i2c")
{
    
}

foxtrot::protocols::i2c::~i2c()
{
    close(_fd);
}


void foxtrot::protocols::i2c::Init(const parameterset *const class_parameters)
{
    foxtrot::CommunicationProtocol::Init(class_parameters);
    
    extract_parameter_value(_devnode,_params,"devnode");
    extract_parameter_value(_address,_params,"address");
    
    
    if(_address > ( (1<<7) -1))
    {
     throw ProtocolError("invalid device address, > 2**7-1");   
    }
    
    _fd = open(_devnode.c_str(),O_RDWR);
    if(_fd < 0)
    {
     throw ProtocolError(std::string("couldn't open devnode: " ) + _devnode + std::string(" ") + strerror(errno));   
    }
    
    unsigned char actaddr = static_cast<unsigned char>(_address);
    
    if(ioctl(_fd, I2C_SLAVE, actaddr) < 0)
    {
        throw ProtocolError("failed to set i2c slave address");
    };
    
    
}

std::vector<unsigned char> foxtrot::protocols::i2c::read_block_data(int cmd, int len)
{
    
    i2c_smbus_ioctl_data ioc;
    
    union i2c_smbus_data dat;
    dat.block[0] = len;
    
    ioc.command = cmd;
    ioc.size = len;
    ioc.data = &dat;
    ioc.read_write = I2C_SMBUS_READ;

    auto ret = ioctl(_fd,I2C_SMBUS ,&ioc);
    
    if(ret ==-1)
    {
        _lg.Error("ioctl failed");
        _lg.Error("errno: " + std::to_string(errno));
        throw ProtocolError("failed to read block data");
    };
    
    std::vector<unsigned char> out(dat.block +1, dat.block + dat.block[0]);
    
    
    
}

void foxtrot::protocols::i2c::write_block_data(int cmd, const std::vector< unsigned char >& data)
{
  //TODO: write this function!
  
  throw std::runtime_error("not implemented yet");
  

}

