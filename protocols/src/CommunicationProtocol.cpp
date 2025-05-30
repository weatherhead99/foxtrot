#include <foxtrot/protocols/CommunicationProtocol.h>
#include <foxtrot/protocols/SerialProtocol.h>
#include <foxtrot/Logging.h>
#include <stdexcept>

foxtrot::SerialProtocol::SerialProtocol(const foxtrot::parameterset* const instance_parameters)
  : CommunicationProtocol(instance_parameters)
  
{
};


string foxtrot::SerialProtocol::read_definite(unsigned, foxtrot::opttimeout)
{
  throw std::logic_error("use of new unimplimented read function");
}


foxtrot::CommunicationProtocol::CommunicationProtocol(const foxtrot::parameterset* const instance_parameters)
  : lg("CommunicationProtocol")
{
  
  if(instance_parameters != nullptr)
   _params = *instance_parameters; 

}

foxtrot::CommunicationProtocol::~CommunicationProtocol()
{

}

void foxtrot::CommunicationProtocol::open()
{
  lg.strm(sl::warning) << "unimplemented open() method called in communication protocol";
}

void foxtrot::CommunicationProtocol::close()
{
  lg.strm(sl::warning) << "unimplemented close() method called in communication protocol";
}

void foxtrot::CommunicationProtocol::Init(const foxtrot::parameterset* const class_parameters)
{
  

  if(class_parameters != nullptr)    
  {
    //merge parameter sets, with a preference for class parameters
    decltype(_params) newparams{*class_parameters};
  
    newparams.insert(_params.begin(), _params.end());
    std::swap(_params,newparams);


  }


}


const foxtrot::parameterset& foxtrot::CommunicationProtocol::GetParameters() const
{
  return _params;

}
