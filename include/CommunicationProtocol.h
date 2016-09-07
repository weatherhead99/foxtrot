#pragma once 

#include <map>
#include <boost/variant.hpp>
#include <string>
#include <boost/mpl/vector.hpp>

#include <utility>

namespace foxtrot
{
  
  using parameter_types = boost::mpl::vector<unsigned,unsigned char, int , std::string>;
  using parameter = boost::variant<unsigned, int, std::string>;
  using parameterset = std::map<std::string, parameter>;
  
  
  class parameter_visit : public boost::static_visitor<>
  {
    
    
    
    
  };
  
  
  class CommunicationProtocol
  {
  public:
    CommunicationProtocol(const parameterset* const instance_parameters);
    virtual ~CommunicationProtocol(); 
    virtual void Init(const parameterset * const class_parameters);
    
    static bool verify_instance_parameters(const parameterset& instance_parameters);
    static bool verify_class_parameters(const parameterset& class_parameters);
    
    const parameterset& GetParameters() const;
    
  protected:
    parameterset _params;
  };
  
  
  
  
};