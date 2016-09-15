#pragma once

#include <string>

#include "CommunicationProtocol.h"


namespace foxtrot
{
 class SerialProtocol : public CommunicationProtocol
 {
 public:
    SerialProtocol(const parameterset*const instance_parameters);
   
   virtual void write(const std::string& data) = 0;
   virtual std::string read(unsigned len, unsigned* actlen=nullptr) = 0;
   
   
   
 };
  
  
}