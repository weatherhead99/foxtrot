#pragma once 
#include <string>

namespace foxtrot
{
  class Client;
  class TelemetryServer;
  class NanomsgTransport;
}

void configure_telemetry_server(const std::string& fname, foxtrot::Client& cl, foxtrot::TelemetryServer& serv);
