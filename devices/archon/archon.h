#pragma once

#include "CmdDevice.h"
#include <memory>
#include <string>
#include "protocols/simpleTCP.h"
#include <map>
#include <vector>


using std::string;
typedef std::map<std::string,std::string> ssmap;


namespace foxtrot {
  
  using namespace protocols;
  
  
  namespace devices
  {
   class ArchonModule;
  
  class archon : public CmdDevice
  {
  public:
    archon(std::shared_ptr<simpleTCP> proto);
    ~archon();
    const ssmap& getStatus() const;
    const ssmap& getSystem() const;
    
    void update_state();
    
    const std::vector<std::unique_ptr<ArchonModule>> getAllModules() const;
    
  protected:
    virtual std::string cmd(const std::string& request) override;
    ssmap parse_parameter_response(const std::string& response);
    
    
  private:
    std::vector<int> get_module_positions(const string& module_occupied_str);
    
    short unsigned _order;
    std::shared_ptr<simpleTCP> _specproto;
    
    ssmap _system;
    ssmap _status;
    
    std::vector<std::unique_ptr<ArchonModule>>* _modules;
    
    
  };
  
};
};
