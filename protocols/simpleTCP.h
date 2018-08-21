#pragma once

#include "SerialProtocol.h"
#include <string>
#include "Logging.h"


using namespace foxtrot;

namespace foxtrot {
  
  namespace protocols
  {

class FOXTROT_EXPORT simpleTCP : public SerialProtocol
{
public:
  
    simpleTCP(const parameterset*const instance_parameters);
    
    virtual ~simpleTCP();
    
    virtual void Init(const parameterset*const class_parameters) override;
    void Init(const unsigned port, const std::string& addr);
    
    
    virtual std::string read(unsigned int len, unsigned* actlen = nullptr) override;
    virtual void write(const std::string& data) override;
    
    virtual std::string read_until_endl(char endlchar = '\n');
    
    void setchunk_size(unsigned chunk);
    unsigned getchunk_size();
    
    unsigned bytes_available();
    
    static bool verify_instance_parameters(const parameterset& instance_parameters);
    static bool verify_class_parameters(const parameterset& class_parameters);
    
private:
    int _chunk_size = 1024;
    int _port;
    std::string _addr;
    
    int _sockfd;
    
    foxtrot::Logging _lg;
    
}; 
  
}; //namespace protocols

}//namespace foxtrot