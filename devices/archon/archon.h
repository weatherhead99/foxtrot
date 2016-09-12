#pragma once

#include "Device.h"

#include <memory>

#include <string>

#include "protocols/simpleTCP.h"


namespace foxtrot {
  
  using namespace protocols;
  
  namespace devices
  {
    

  class archon : public Device
  {
    
  public:
    archon(std::shared_ptr<simpleTCP> proto);
    
  protected:
    std::string archoncmd(const std::string& request);
    
  
  private:
    short unsigned _order;
    std::shared_ptr<simpleTCP> _specproto;

  };
  
  
};

};