#include <iostream>

#include <errno.h>
#include <string.h>
#include <sstream>

#include <foxtrot/protocols/ProtocolUtilities.h>
#include <foxtrot/protocols/characterdevice.h>

using std::cout;
using std::endl;

foxtrot::protocols::characterdevice::characterdevice(const foxtrot::parameterset*const instance_parameters)
: SerialProtocol(instance_parameters)
{
  //set fstream to throw exceptions on fail and bad
  _fs.exceptions(fstream::failbit | fstream::badbit);

}



foxtrot::protocols::characterdevice::~characterdevice()
{
  
  _fs.close();

}

void foxtrot::protocols::characterdevice::Init(const foxtrot::parameterset*const class_parameters)
{
//   call base to merge parameter sets
  foxtrot::CommunicationProtocol::Init(class_parameters);

  extract_parameter_value(_devnode, _params, "devnode");

  _fs.open(_devnode, fstream::in | fstream::out);
  
  
  
  
}



std::string foxtrot::protocols::characterdevice::read(unsigned int len, unsigned int* actlen)
{
 std::ostringstream oss;
 
 oss << _fs.rdbuf();
  return oss.str();
}

void foxtrot::protocols::characterdevice::write(const std::string& data)
{
//   cout << "writing..." << endl;
  _fs << data << std::endl;
//   cout << "flushing..." << endl;
  _fs.flush();
//   cout << "flushed." << endl;
  
}


std::string foxtrot::protocols::characterdevice::read_until_endl(char endlchar)
{
  string repl;
  std::getline(_fs,repl,endlchar);
  
  return repl;
}

