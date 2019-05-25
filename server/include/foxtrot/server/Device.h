#pragma once

#include <memory>
#include <string>
#include <rttr/type>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <foxtrot/foxtrot_server_export.h>
#include <foxtrot/Logging.h>
#define THIS_TYPE std::remove_reference<decltype(*this)>::type


namespace foxtrot
{
    using ft_returntype = boost::optional<boost::variant<double,int,bool,std::string>>;
    using ft_argtype = boost::variant<double,int,bool,std::string>;
    using arg_cit = std::vector<ft_argtype>::const_iterator;
    using rarg_cit = std::vector<rttr::variant>::const_iterator;
    
 enum class CapabilityMeta
 {
  STREAMINGDATA 
     
 };
 
 enum class CapabilityType
 {
     VALUE_READONLY,
     VALUE_READWRITE,
     ACTION,
     STREAM 
 };

  struct Capability
  {
      CapabilityType type;
      std::string CapabilityName;
      std::vector<std::string> Argnames;
      std::vector<rttr::type> Argtypes;
      rttr::type Returntype = rttr::type::get<void>();
  };
 
 
  class CommunicationProtocol;
  
  class FOXTROT_SERVER_EXPORT Device
  {
    RTTR_ENABLE()
  public:
    Device(std::shared_ptr<CommunicationProtocol> proto, const std::string& comment = "");
    
    virtual const std::string getDeviceTypeName() const;
    const std::string getDeviceComment() const;
    void setDeviceComment(const std::string& comment);
    virtual std::vector<std::string> GetCapabilityNames() const;
    virtual ft_returntype Invoke(const std::string& capname, arg_cit beginargs, arg_cit endargs);
    virtual rttr::variant Invoke(const std::string& capname, rarg_cit beginargs, rarg_cit endargs);
    virtual Capability GetCapability(const std::string& capname) const;
    
  protected:
    std::shared_ptr<CommunicationProtocol> _proto;
    std::string _devcomment;
  private:
    foxtrot::Logging lg_;
    
    
  };


}
