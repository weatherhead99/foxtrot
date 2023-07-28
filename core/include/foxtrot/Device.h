#pragma once

#include <memory>
#include <string>
#include <rttr/type>
#include <foxtrot/Logging.h>
#include <mutex>
#include <optional>


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
    Device(std::shared_ptr<CommunicationProtocol> proto, const std::string& comment = "");
    virtual ~Device();
    
    virtual const std::string getDeviceTypeName() const;
    const std::string getDeviceComment() const;
    void setDeviceComment(const std::string& comment);
    virtual std::vector<std::string> GetCapabilityNames() const;
    virtual rttr::variant Invoke(const std::string& capname, rarg_cit beginargs, rarg_cit endargs);
    virtual rttr::variant Invoke(const Capability& cap,
                                 rarg_cit beginargs, rarg_cit endargs);
    
    virtual Capability GetCapability(const std::string& capname) const;
    
    virtual std::optional<Lock> obtain_lock(const Capability& cap);
    virtual bool hasLockImplementation() const;
    
    
  protected:
    std::shared_ptr<CommunicationProtocol> _proto;
    std::string _devcomment;
  private:
    foxtrot::Logging lg_;
    
    
  };


}
