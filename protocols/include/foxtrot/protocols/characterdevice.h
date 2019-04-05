#pragma once

#include <string>
#include <fstream>

#include <foxtrot/foxtrot_server_export.h>
#include <foxtrot/protocols/SerialProtocol.h>

using std::string;
using std::fstream;

namespace foxtrot {
 namespace protocols {
   
   
   class FOXTROT_SERVER_EXPORT characterdevice : public SerialProtocol
   {
   public:
     characterdevice(const parameterset* const instance_parameters);
     virtual ~characterdevice();
     
     virtual void Init(const parameterset* const class_parameters);
     
     virtual std::string read(unsigned int len, unsigned int* actlen = nullptr);
     virtual void write(const std::string& data);

     virtual std::string read_until_endl(char endlchar = '\n');
     
     
   private:
     string _devnode;
     fstream _fs;
     
     
   };
   
   
 } 
}
