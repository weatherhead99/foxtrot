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
    
    std::string fetch_log();
    std::vector<std::string> fetch_all_logs();
    
    
    int writeConfigLine(const std::string& line, int num=-1);
    std::string readConfigLine(int num);
    
    
    void writeKeyValue(const std::string& key, const std::string& val);
    std::string readKeyValue(const std::string& key);
    
    
    void update_state();
    void applyall();
    void applymodule(int modpos);
    
    const std::map<int,ArchonModule&> getAllModules() const;
    
    
    
  protected:
    virtual std::string cmd(const std::string& request) override;
    ssmap parse_parameter_response(const std::string& response);
    
    
  private:
    
    short unsigned _order;
    std::shared_ptr<simpleTCP> _specproto;
    
    ssmap _system;
    ssmap _status;
    
    std::map<int, std::unique_ptr<ArchonModule>> _modules;
    
    std::map<const std::string, int> _configlinemap;
    
    int _config_lines =0;
    
    
  };
  
};
};
