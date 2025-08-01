#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <mutex>
#include <unordered_map>

#include <boost/iterator/iterator_concepts.hpp>
#include <boost/date_time.hpp>

#include <rttr/registration>

#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/simpleTCP.h>
#include <foxtrot/Logging.h>
#include "../device_utils/stringconv_utils.hh"

#include "archon_module_mapper.hh"


using std::optional;
using std::string;
using std::vector;
using std::unordered_map;
typedef std::unordered_map<std::string,std::string> ssmap;


namespace foxtrot {
  
  using namespace protocols;
  using foxtrot::protocols::simpleTCPBase;
  
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

    namespace detail {
      template<typename T>
      concept ArchonNumeric = std::is_integral_v<T> || std::is_floating_point_v<T>;
      struct archonimpl;

    }

    using HRTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    enum class archon_buffer_mode : short unsigned
      {
	top = 0,
	bottom = 1,
	split = 2
      };

    enum class archon_sample_mode : short unsigned
      {
	bit16 = 0,
	bit32 = 1
      };

    
    struct archon_buffer_info
    {
      archon_sample_mode sampmode;
      bool complete;
      archon_buffer_mode bufmode;
      unsigned long offsetaddr;
      unsigned frame_number;
      unsigned width;
      unsigned height;
      unsigned pixel_progress;
      unsigned line_progress;
      unsigned raw_blocks;
      unsigned raw_lines;
      unsigned long raw_offset;
      HRTimePoint timestamp;
      HRTimePoint REtimestamp;
      HRTimePoint FEtimestamp;
      HRTimePoint REAtimestamp;
      HRTimePoint FEAtimestamp;
      HRTimePoint REBtimestamp;
      HRTimePoint FEBtimestamp;
    };


    struct archon_frame_info
    {
      HRTimePoint current_time;
      unsigned short rbuf;
      unsigned short wbuf;
      std::array<archon_buffer_info, 3> buffer_infos;
    };

    struct archon_system_info
    {
      unsigned backplane_type;
      unsigned backplane_rev;
      string backplane_version;
      unsigned long long backplane_id;
      unsigned long long power_id;
      std::vector<archon_module_info> modules;
    };

    struct archon_tap_info
    {
      short unsigned pixels;
      short unsigned lines;
      short unsigned rawchannel;
      bool rawenable;
      short unsigned rawstartline;
      short unsigned rawendline;
      unsigned rawstartpixel;
      unsigned rawsamples;
      archon_buffer_mode framemode;
      archon_sample_mode samplemode;
      std::array<short unsigned, 4> CDSTiming;
    };

    enum class archon_power_status : short unsigned
      {
	unknown = 0,
	not_configured = 1,
	off = 2,
	intermediate = 3,
	on = 4,
	standby = 5
      };

    //NOTE not implementing heater for now (not needed for DEIMOS)
    struct archon_module_status
    {
      double temp;
      optional<unsigned> dinput_status;
      optional<vector<double>> HC_Vs;
      optional<vector<double>> HC_Is;
      optional<vector<double>> LC_Vs;
      optional<vector<double>> LC_Is;
      optional<vector<double>> XVN_Vs;
      optional<vector<double>> XVP_Vs;
      optional<vector<double>> XVN_Is;
      optional<vector<double>> XVP_Is;
      optional<vector<unsigned>> vcpu_outreg;

    };

    
    struct archon_status
    {
      bool valid;
      unsigned count;
      short unsigned nlogs;
      archon_power_status powerstatus;
      bool powergood;
      bool overheat;
      double backplane_temp;
      std::unordered_map<string, std::pair<double,double>> PSU_map;
      std::optional<unsigned> fanspeed = std::nullopt;
      std::vector<archon_module_status> module_statuses;
    };

    //fwd declares
   class ArchonModule;


  class archon : public CmdDevice
  {
      RTTR_ENABLE(CmdDevice)
    friend class foxtrot::devices::ArchonModule;
    virtual const string getDeviceTypeName() const override;
  public:

    static std::shared_ptr<archon> create(std::shared_ptr<simpleTCPBase>&& proto, bool clear_config=true);

    ~archon();
    ssmap getStatus();
    ssmap getSystem();
    ssmap getFrame();

    archon_status status();
    archon_frame_info frameinfo();
    archon_system_info system();
    
    void clear_config();
    
    std::string fetch_log();
    std::vector<std::string> fetch_all_logs();

    void writeKeyValue(const std::string& key, const std::string& val);

    template<detail::ArchonNumeric T>
    void writeKeyValue(const std::string& key, T&& val)
    {
      writeKeyValue(key, std::to_string(val));
    }

    const std::string& readKeyValue(const std::string& key);
    const std::string* const readKeyValueOpt(const std::string& key);

    template<typename Ret>
    Ret readKeyValue(const std::string& key)
    {
      const auto& val = readKeyValue(key);
      return number_from_string<Ret>(val);
    }
    
    void applyall();

    std::vector<unsigned int> fetch_buffer(int buf);
    std::vector<unsigned short> fetch_raw_buffer(int buf);
    
    archon_tap_info tapinfo();
    void set_tapinfo(const archon_tap_info& tapinfo);
    
    const std::map<int,ArchonModule&> getAllModules() const;
    
    
    //void set_timing_lines(int n);
    //int get_timing_lines();
    
    //void set_states(int n);
    //int get_states();
    
    //void set_parameters(int n);
    //int get_parameters();

    void load_config(const std::string& cfg);
    
    const std::unordered_map<std::string, std::string>& config() const;
    std::vector<std::pair<std::string, std::string>> ordered_config() const;

    std::unordered_map<std::string, int> params();
    void set_param(const std::string& name, int val, bool apply_immediate=true, bool allow_new=false);
    
    //void set_constants(int n);
    //int get_constants();
    
    void set_power(bool onoff);
    archon_power_status get_power();
    
    void load_timing_script(const std::string& script);
    void load_timing();
    
    //void write_timing_state(std::string name, const std::string& state);
    
    void lockbuffer(int buf);
    void unlockbuffers();
    
    std::string readConfigLine(unsigned num,bool override_existing=false);

    
    void holdTiming();
    void releaseTiming();
    void resetTiming();

    
    //void setParam(const std::string& name, unsigned val);
    //unsigned getParam(const std::string& name);
    //unsigned getParam(int loc);
    
    //void setConstant(const std::string& name, double val);
    //double getConstant(const std::string& name);
    
    void apply_param(const std::string& name);
    void apply_all_params();
    
    void sync_archon_timer();
    HRTimePoint get_archon_time();
    void setCDSTiming(int reset_start, int reset_end, int signal_start, int signal_end);

    std::array<int,4> getCDSTiming();

    void settapline(int n, const std::string& tapline);
    void settap(unsigned char AD, bool LR, double gain, unsigned short offset);
    void setAMtap(unsigned char AD, bool LR, double gain, unsigned short offset);
    
    void settrigoutpower(bool onoff);
    bool gettrigoutpower();
    
    void settrigoutinvert(bool invert);
    bool gettrigoutinvert();
    
    void settrigoutlevel(bool onoff);
    bool gettrigoutlevel();
    
    void settrigoutforce(bool onoff);
    bool gettrigoutforce();

    void read_parse_existing_config(bool allow_empty=false);
    
    
    
  protected:
    //NOTE: archon constructor is private, archon must only exist as a shared_ptr
    //(it is shared with its modules etc)
     archon(std::shared_ptr<foxtrot::protocols::simpleTCPBase> proto);

    virtual std::string cmd(const std::string& request) override;
    ssmap parse_parameter_response(const std::string& response);
    
    std::vector<unsigned char> parse_binary_response(const std::string& response);
    
    //NOTE: should this be a timed mutex?
    std::mutex _cmdmut;
    
    std::shared_ptr<foxtrot::protocols::simpleTCPBase> _specproto;
    foxtrot::Logging _lg;
    std::map<int, std::unique_ptr<ArchonModule>> _modules;
    unsigned long long _arch_tmr;
    HRTimePoint _sys_tmr;
    void setup_modules();

    HRTimePoint archon_time_to_real_time(long long unsigned archon_time) const;

    
    
  private:
    std::optional<int> find_config_line_from_key(const std::string& key);
    
    
    //NOTE: readConfigLine and writeConfigLine are unsafe
    //to call because they cause the config line map
    //to get out of sync!
    int writeConfigLine(const std::string& line, int num=-1);

    
    
    std::unique_ptr<detail::archonimpl> impl;   
     
      template<typename T, typename Tdiff=T>
      std::vector<T> read_back_buffer(int num_blocks, int retries, unsigned address);


    short unsigned _order;

    std::unordered_map<std::string, std::string> _configmap;
    std::vector<std::string> _statenames;
    std::map<unsigned char, unsigned char> _ADtaplinemap;
    std::optional<bool> _using_AM_taps = std::nullopt;
    
  };

  class ArchonStreamHelper
  {
  public:
    ArchonStreamHelper(archon& dev);
    ArchonStreamHelper& operator<<(auto& right)
    {
      _oss << right;
      return *this;
    }

    template<typename Ret>
    Ret read() {
      auto retstr =  _dev.readKeyValue(_oss.str());

      using Ret2 = std::decay_t<Ret>;
      
      if constexpr(std::is_same_v<Ret2, double>) { return std::stod(retstr);}
      else if constexpr(std::is_same_v<Ret2, int>) {return std::stoi(retstr);}
      else
	throw std::logic_error("unknown type in archon stream helper");	
    };

    void write(auto& val) {
      _dev.writeKeyValue(_oss.str(), std::to_string(val)); }

  private:
    archon& _dev;
    std::ostringstream _oss;

    
  };



  
};
};
