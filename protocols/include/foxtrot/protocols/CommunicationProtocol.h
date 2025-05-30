#pragma once 

#include <foxtrot/parameter_defs.hh>
#include <foxtrot/Logging.h>
#include <utility>

namespace foxtrot
{
  
  
  
  // class parameter_visit : public std::static_visitor<>
  // {
  // };
  
  
  class  CommunicationProtocol
  {
  public:
    CommunicationProtocol(const parameterset* const instance_parameters);
    virtual ~CommunicationProtocol(); 
    virtual void Init(const parameterset * const class_parameters);

    virtual void open();
    virtual void close();

    static bool verify_instance_parameters(const parameterset& instance_parameters);
    static bool verify_class_parameters(const parameterset& class_parameters);
    
    const parameterset& GetParameters() const;
    
  protected:
    parameterset _params;
  private:
    foxtrot::Logging lg;
  };
  
  
  
  
};
