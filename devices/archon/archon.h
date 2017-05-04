
#pragma once

#include "CmdDevice.h"
#include <memory>
#include <string>
#include "protocols/simpleTCP.h"
#include <map>
#include <vector>
#include <boost/iterator/iterator_concepts.hpp>

#include <rttr/registration>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "Logging.h"

using std::string;
typedef std::map<std::string,std::string> ssmap;


namespace foxtrot {
  
  using namespace protocols;
  
  struct framemeta
  {
   unsigned long long u64timestamp;
   unsigned frameno;
   unsigned width;
   unsigned height;
   unsigned baseaddr;
   bool bit32;   
  };
  
  
  namespace devices
  {
    //fwd declares
   class ArchonModule;
   enum class archon_module_types : short unsigned;
   
   
  class archon : public CmdDevice
  {
      RTTR_ENABLE(CmdDevice)
    friend class foxtrot::devices::ArchonModule;
    virtual const string getDeviceTypeName() const;
  public:
    archon(std::shared_ptr<simpleTCP> proto);
    ~archon();
    const ssmap& getStatus() const;
    const ssmap& getSystem() const;
    const ssmap& getFrame() const;
    
    void clear_config();
    
    std::string fetch_log();
    std::vector<std::string> fetch_all_logs();
    
    
    int writeConfigLine(const std::string& line, int num=-1);
    std::string readConfigLine(int num,bool override_existing=false);
    
    
    void writeKeyValue(const std::string& key, const std::string& val);
    std::string readKeyValue(const std::string& key);
    
    
    void update_state();
    void applyall();

    std::vector<unsigned int> fetch_buffer(int buf);
    
    int get_frameno(int buf);
    int get_width(int buf);
    int get_height(int buf);
    int get_mode(int buf);
    bool get_32bit(int buf);
    
    
    
    const std::map<int,ArchonModule&> getAllModules() const;
    
    
    void set_timing_lines(int n);
    int get_timing_lines();
    
    void set_states(int n);
    int get_states();
    
    void set_parameters(int n);
    int get_parameters();
    
    void set_constants(int n);
    int get_constants();
    
    void set_power(bool onoff);
    bool get_power();
    
    void load_timing_script(const std::string& script);
    
    void write_timing_state(const std::string& name, const std::string& state);
    
    
    
    void lockbuffer(int buf);
    void unlockbuffers();
    
    bool isbuffercomplete(int buf);
    
    void holdTiming();
    void releaseTiming();
    void resetTiming();
    
    void setParam(const std::string& name, unsigned val);
    unsigned getParam(const std::string& name);
    
    void setConstant(const std::string& name, unsigned val);
    unsigned getConstant(const std::string& name);
    
    void apply_param(const std::string& name);
    void apply_all_params();
    
    
    void sync_archon_timer();
    
    void setCDSTiming(int reset_start, int reset_end, int signal_start, int signal_end);
    
    int getreset_start();
    int getreset_end();
    int getsignal_start();
    int getsignal_end();
    
    
    void settapline(int n, const std::string& tapline);
    
    void settrigoutpower(bool onoff);
    void settrigoutinvert(bool invert);
    
    
    void setrawenable(bool onoff);
    void setrawchannel(int ch);
    void setrawstartline(int line);
    void setrawendline(int line);
    void setrawstartpixel(int pix);
    void setrawsamples(int n);
    
    
    
  protected:
    virtual std::string cmd(const std::string& request) override;
    ssmap parse_parameter_response(const std::string& response);
    
    std::vector<unsigned char> parse_binary_response(const std::string& response);
    
    
  private:
      void read_parse_existing_config();
    
    short unsigned _order;
    std::shared_ptr<simpleTCP> _specproto;
    
    ssmap _system;
    ssmap _status;
    ssmap _frame;
    
    std::map<unsigned char, framemeta> _framedata;
    std::map<int, std::unique_ptr<ArchonModule>> _modules;
    
    std::map<std::string, int> _configlinemap;
     
    std::vector<std::string> _statenames;
    
    
    int _config_lines =0;
    int _taplines = 0;
    int _states = 0;
    
    unsigned long long _arch_tmr;
    boost::posix_time::ptime _sys_tmr;
    foxtrot::Logging _lg;
    
  };
  
};
};
