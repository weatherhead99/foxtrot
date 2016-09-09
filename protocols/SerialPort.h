#pragma once
#include "SerialProtocol.h"
#include <string>

namespace foxtrot
{
  
  namespace protocols
  {
    
    class SerialPort : public SerialProtocol
    {
    public:
    SerialPort(const parameterset*const instance_parameters);
    ~SerialPort();
    virtual void Init(const parameterset*const class_parameters) override;
    virtual std::string read(unsigned int len) override;
    virtual void write(const std::string& data) override;
      
    private:
      int _fd;
      std::string _port;
      unsigned _baudrate;
      
    };
    
    
  }//namespace protocols
  
}//namespace foxtrot