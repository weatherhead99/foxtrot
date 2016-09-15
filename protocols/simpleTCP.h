#pragma once

#include "SerialProtocol.h"
#include <string>

using namespace foxtrot;

namespace foxtrot {
  
  namespace protocols
  {

class simpleTCP : public SerialProtocol
{
public:
  
    simpleTCP(const parameterset*const instance_parameters);
    
    virtual ~simpleTCP();
    
    virtual void Init(const parameterset*const class_parameters) override;
    void Init(const unsigned port, const std::string& addr);
    
    
    virtual std::string read(unsigned int len, unsigned* actlen = nullptr) override;
    virtual void write(const std::string& data) override;
    
    static bool verify_instance_parameters(const parameterset& instance_parameters);
    static bool verify_class_parameters(const parameterset& class_parameters);
    
private:
    unsigned _port;
    std::string _addr;
    
    int _sockfd;
    
}; 
  
}; //namespace protocols

}//namespace foxtrot