#include "CommunicationProtocol.h"

foxtrot::CommunicationProtocol::CommunicationProtocol(const foxtrot::parameterset* const instance_parameters) 
{
  
  if(instance_parameters != nullptr)
  {
   _params = *instance_parameters; 
  }

}

foxtrot::CommunicationProtocol::~CommunicationProtocol()
{

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
