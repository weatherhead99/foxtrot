#pragma once
#include <string>
#include "Logging.h"
#include <boost/date_time.hpp>
#include "foxtrot.pb.h"
#include <sstream>
#include "client.h"

namespace foxtrot{

  
  struct telemetry_message
  {
    std::string name;
    boost::posix_time::ptime timestamp;
    ft_variant value;
  };
  
  
  
class TelemetryClient
{
public:
  TelemetryClient();
  ~TelemetryClient();
  
  void ConnectSocket(const std::string& connstr);
  void Subscribe(const std::string& topicstr);
  
  telemetry_message waitMessageSync();
  
  
private:
  foxtrot::telemetry decode_from_string(const std::string& instr);
  
  
  int _nn_sub_skt;
  foxtrot::Logging _lg;

};

  
}