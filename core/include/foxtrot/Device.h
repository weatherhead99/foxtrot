#pragma once

#include <memory>
#include <string>
#include <rttr/type>
#include <foxtrot/Logging.h>
#include <mutex>
#include <optional>
#include <variant>
#include <map>


#define THIS_TYPE std::remove_reference<decltype(*this)>::type


namespace foxtrot
{
    using rarg_cit = std::vector<rttr::variant>::const_iterator;
    using Lock = std::unique_lock<std::timed_mutex>;
    
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
      std::string CapabilityName;
      std::vector<std::string> Argnames;
      std::vector<rttr::type> Argtypes;
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
    RTTR_ENABLE()
  public:
    Device(std::shared_ptr<CommunicationProtocol> proto, const std::string& comment = "", bool load_capabilities=true);
    
    virtual const std::string getDeviceTypeName() const;
    const std::string getDeviceComment() const;
    void setDeviceComment(const std::string& comment);
    virtual std::vector<std::string> GetCapabilityNames() const;
    virtual rttr::variant Invoke(const std::string& capname, rarg_cit beginargs, rarg_cit endargs);
    virtual rttr::variant Invoke(const Capability& cap,
                                 rarg_cit beginargs, rarg_cit endargs);

    virtual rttr::variant Invoke(const std::variant<rttr::property, rttr::method>& invokable,
                                 rarg_cit beginargs, rarg_cit endargs);

    virtual rttr::variant Invoke(unsigned short capid, rarg_cit beginargs, rarg_cit endargs);

    virtual Capability GetCapability(const std::string& capname) const;

    virtual std::optional<Lock> obtain_lock(const Capability& cap);
    virtual bool hasLockImplementation() const;
    
    virtual bool Reconnect();
    virtual bool Reset();

    virtual ~Device();
    
    
  protected:
    std::shared_ptr<CommunicationProtocol> _proto;
    std::string _devcomment;

    void load_capability_map(bool force_reload=false);


    template<typename PropMeth>
    Capability GetCapability(PropMeth&& pm) const
    {
        Capability out;
        out.CapabilityName = std::string{pm.get_name()};
        out.invokable = pm;

        if constexpr(std::is_same_v<PropMeth, rttr::property>)
        {
            out.Returntype = pm.get_type();

            if(is_ft_call_streaming(pm))
                out.type=CapabilityType::STREAM;
            else if(pm.is_readonly())
                out.type = CapabilityType::VALUE_READONLY;
            else
            {
                out.type = CapabilityType::VALUE_READWRITE;
                out.Argnames.push_back(std::string{out.Returntype.get_name()});
                out.Argtypes.push_back(out.Returntype);
            }
        }
        else if constexpr(std::is_same_v<PropMeth, rttr::method>)
        {
            if(is_ft_call_streaming(pm))
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
                out.Argnames.push_back(std::string{arg.get_name()});
                out.Argtypes.push_back(arg.get_type());
            }
        }
        else
            static_assert("incompatible type passed!");

        return out;
    }


  private:
    foxtrot::Logging lg_;
    std::map<unsigned short,Capability> _cap_registry;
    bool _registry_is_loaded = false;
    std::map<std::string, unsigned short> _cap_string_registry;
    
  };


}
