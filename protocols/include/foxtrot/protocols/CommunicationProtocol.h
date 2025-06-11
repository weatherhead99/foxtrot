#pragma once 

#include <foxtrot/parameter_defs.hh>
#include <foxtrot/Logging.h>
#include <utility>
#include <optional>

namespace foxtrot
{


  class  CommunicationProtocol
  {
  public:
    CommunicationProtocol(const parameterset* const instance_parameters);

    template<typename... Ts>
    CommunicationProtocol(std::pair<const char*, Ts>  ...pargs)
      : lg("CommunicationProtocol")
    { 
      (_params.insert_or_assign(std::get<0>(pargs), std::get<1>(pargs)), ...);

    }; 

    virtual ~CommunicationProtocol(); 
    virtual void Init(const parameterset * const class_parameters,
		      bool open_immediate=true);

    virtual void open();
    virtual void close();

    virtual std::optional<bool> is_open();

    static bool verify_instance_parameters(const parameterset& instance_parameters);
    static bool verify_class_parameters(const parameterset& class_parameters);

    const parameterset& GetParameters() const;

  protected:
    parameterset _params;
  private:
    foxtrot::Logging lg;
  };


};
