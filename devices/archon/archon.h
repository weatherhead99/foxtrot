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
    //fwd declares
   class ArchonModule;
   enum class archon_module_types : short unsigned;
   
   
  class archon : public CmdDevice
  {
  public:
    archon(std::shared_ptr<simpleTCP> proto);
    ~archon();
    const ssmap& getStatus() const;
    const ssmap& getSystem() const;
    
    void clear_config();
    
    void writeConfigLine(const std::string& line);
    std::string readConfigLine(int num);
    
    void update_state();
    void applyall();
    
    const std::map<int,const ArchonModule&> getAllModules() const;
    
    
    
  protected:
    virtual std::string cmd(const std::string& request) override;
    ssmap parse_parameter_response(const std::string& response);
    
    
  private:
    static std::unique_ptr<ArchonModule> constructModule(const archon_module_types& type, int modpos);
    
    short unsigned _order;
    std::shared_ptr<simpleTCP> _specproto;
    
    ssmap _system;
    ssmap _status;
    
    std::map<int, std::unique_ptr<ArchonModule>>* _modules;
    
    int _config_lines =0;
    
    
  };
  
};
};
