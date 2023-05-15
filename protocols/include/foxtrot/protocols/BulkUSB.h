#pragma once
#include <foxtrot/Logging.h>
#include <foxtrot/protocols/libUsbProtocol.hh>

#include <memory>

namespace foxtrot
{
  namespace protocols
  {
    class  BulkUSB : public libUsbProtocol
    {
    public:
    BulkUSB(const parameterset*const instance_parameters);
    virtual ~BulkUSB();
      
    virtual std::string read(unsigned int len, unsigned* actlen = nullptr) override;
    virtual void write(const std::string& data) override;
    
    void clear_halts();
    
    private:
      foxtrot::Logging _lg;
    };
    
    
  }//namespace protocols
  
  
}//namespace foxtrot
