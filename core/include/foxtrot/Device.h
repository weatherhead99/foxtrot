#pragma once

#include <memory>
#include <string>
#include <rttr/type>
#include <foxtrot/Logging.h>
#include <mutex>
#include <optional>
#include <variant>
#include <unordered_map>
#include <type_traits>


using std::vector;
using std::string;

#define THIS_TYPE std::remove_reference<decltype(*this)>::type

namespace detail
{
template<typename T> bool is_ft_call_streaming ( const T& propmeth )
{
    auto streammeta = propmeth.get_metadata ( "streamdata" );
    if ( streammeta.is_valid() ) {
        if ( streammeta.to_bool() ) {
            return true;
        };

    };
    return false;
};

}



namespace foxtrot
{
    using rarg_cit = vector<rttr::variant>::const_iterator;
    using Lock = std::unique_lock<std::timed_mutex>;


  class DeviceHarness;
  
 enum class CapabilityMeta
 {
  STREAMINGDATA 
     
 };
 
 enum class CapabilityType : unsigned char
 {
     VALUE_READONLY,
     VALUE_READWRITE,
     ACTION,
     STREAM,
     CONSTRUCT
 };


  enum class CapabilityFlags:  unsigned long
  {
    REMOTE_STORE_OBJECT = 0x01

  };

  struct Capability
  {
      CapabilityType type;
      string CapabilityName;
      std::optional<unsigned short> id = std::nullopt;
      vector<string> Argnames;
      vector<rttr::type> Argtypes;
      rttr::type Returntype = rttr::type::get<void>();

      bool operator==(const Capability& other);
      std::optional<std::variant<rttr::property, rttr::method>> invokable = std::nullopt;

    unsigned long flags = 0;
    unsigned long flagmask = 0;

  };
 
  struct CapabilityHash
  {
      std::size_t operator() (const Capability& cap) const noexcept;
  };
  
 
  class CommunicationProtocol;
  
  class  Device : public std::enable_shared_from_this<Device>
  {

    friend class DeviceHarness;
    
    RTTR_ENABLE()
  public:
    Device(std::shared_ptr<CommunicationProtocol> proto, const string& comment = "", bool load_capabilities=false);
    
    virtual const string getDeviceTypeName() const;
    const string getDeviceComment() const;
    void setDeviceComment(const string& comment);

    [[deprecated]] virtual std::vector<string> GetCapabilityNames() const;
    [[deprecated]] virtual rttr::variant Invoke(const string& capname, rarg_cit beginargs, rarg_cit endargs);
    virtual rttr::variant Invoke(const Capability& cap,
                                 rarg_cit beginargs, rarg_cit endargs);

    virtual rttr::variant Invoke(const std::variant<rttr::property, rttr::method>& invokable,
                                 rarg_cit beginargs, rarg_cit endargs);

    virtual rttr::variant Invoke(unsigned short capid, rarg_cit beginargs, rarg_cit endargs);

    [[deprecated]] virtual Capability GetCapability(const string& capname) const;
    virtual Capability GetCapability(short unsigned capid) const;
    virtual vector<unsigned short> GetCapabilityIds(const string& capnmae);
    virtual vector<Capability> GetCapabilities(const string& capname);

    virtual std::optional<Lock> obtain_lock(const Capability& cap);
    virtual bool hasLockImplementation() const;
    
    virtual bool Reconnect();
    virtual bool Reset();

    virtual ~Device();

    const std::unordered_map<unsigned short, Capability>& Registry() const;
    
    
  protected:
    std::shared_ptr<CommunicationProtocol> _proto;
    string _devcomment;

    void load_capability_map(bool force_reload=false);


    template<typename PropMeth>
    Capability GetCapability(PropMeth&& pm) const
    {
        Capability out;
        out.CapabilityName = string{pm.get_name()};
        out.invokable = pm;


	using ActT =  std::decay_t<decltype(pm)>;
	
	
        if constexpr(std::is_same_v<ActT, rttr::property>)
        {
            out.Returntype = pm.get_type();

            if(pm.is_readonly())
                out.type = CapabilityType::VALUE_READONLY;
            else
            {
                out.type = CapabilityType::VALUE_READWRITE;
                out.Argnames.push_back(string{out.Returntype.get_name()});
                out.Argtypes.push_back(out.Returntype);
            }
        }
        else if constexpr(std::is_same_v<ActT, rttr::method>)
        {
	  if(::detail::is_ft_call_streaming(pm))
                out.type = CapabilityType::STREAM;
            else
                out.type = CapabilityType::ACTION;

            auto args = pm.get_parameter_infos();
            auto rettp = pm.get_return_type();
            out.Returntype = rettp;
            out.Argnames.reserve(args.size());
            out.Argtypes.reserve(args.size());

            for(auto& arg: args)
            {
                out.Argnames.push_back(string{arg.get_name()});
                out.Argtypes.push_back(arg.get_type());
            }
        }
        else
            static_assert("incompatible type passed!");

        return out;
    }


  private:
    foxtrot::Logging lg_;
    std::unordered_map<unsigned short,Capability> _cap_registry;
    bool _registry_is_loaded = false;
    std::unordered_multimap<string, unsigned short> _cap_string_registry;
    
  };


}
