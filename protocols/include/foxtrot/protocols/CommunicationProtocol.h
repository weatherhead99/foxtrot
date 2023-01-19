#pragma once 

#include <map>
#include <boost/variant.hpp>
#include <string>
#include <boost/mpl/vector.hpp>

#include <utility>
#include <foxtrot/foxtrot_server_export.h>

namespace foxtrot
{
  
  using parameter_types = boost::mpl::vector<unsigned,char, int , std::string>;
  using parameter_types_minimal = boost::mpl::vector<int,std::string>;
  using parameter = boost::variant<int, std::string>;
  using parameterset = std::map<std::string, parameter>;
  
  
  class parameter_visit : public boost::static_visitor<>
  {
    
        
    
  };
  
  
  class FOXTROT_SERVER_EXPORT CommunicationProtocol
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
