#pragma once 

#include <map>
#include <variant>
#include <string>
#include <boost/mpl/vector.hpp>

#include <utility>

namespace foxtrot
{
  
  using parameter_types = boost::mpl::vector<unsigned,char, int , std::string>;
  using parameter_types_minimal = boost::mpl::vector<int,std::string>;
  using parameter = std::variant<int, unsigned, std::string>;
  using parameterset = std::map<std::string, parameter>;
  
  
  // class parameter_visit : public std::static_visitor<>
  // {
  // };
  
  
  class  CommunicationProtocol
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
