#pragma once
#include <string>

#include <foxtrot/protocols/CommunicationProtocol.h>


namespace foxtrot
{
 class FOXTROT_SERVER_EXPORT SerialProtocol : public CommunicationProtocol
 {
 public:
    SerialProtocol(const parameterset*const instance_parameters);
   
   virtual void write(const std::string& data) = 0;
   virtual std::string read(unsigned len, unsigned* actlen=nullptr) = 0;
    
   virtual std::string read_until_endl( char endlchar = '\n') ;
   
   
   
 };
  
  
}
