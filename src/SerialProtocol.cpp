#include "SerialProtocol.h"
#include <iostream>

foxtrot::SerialProtocol::SerialProtocol(const foxtrot::parameterset*const instance_parameters)
: CommunicationProtocol(instance_parameters)
{

}

std::string foxtrot::SerialProtocol::read_until_endl(char endlchar)
{
  std::cout << "called unimplemented function read_until_endl" << std::endl;
  return std::string("");

}

