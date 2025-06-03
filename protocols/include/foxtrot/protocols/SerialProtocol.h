#pragma once
#include <string>
#include <future>
#include <foxtrot/protocols/CommunicationProtocol.h>
#include <atomic>
#include <optional>
#include <chrono>

using std::chrono::milliseconds;
using std::nullopt;
using std::optional;
using std::string;


namespace foxtrot
{
  using opttimeout = std::optional<milliseconds>;
  
  class  SerialProtocol : public CommunicationProtocol
  {
   
 public:
    SerialProtocol(const parameterset*const instance_parameters);

    template<typename... Ts>
    SerialProtocol(Ts&& ...pargs)
      : CommunicationProtocol(std::forward<Ts>(pargs)...) {};
   
   virtual void write(const string& data) = 0;
   virtual string read_definite(unsigned len, opttimeout wait=nullopt);

   virtual string read(unsigned len, unsigned* actlen=nullptr) = 0;
   virtual string read_until_endl( char endlchar = '\n') = 0;

   
 };
  
  
}
