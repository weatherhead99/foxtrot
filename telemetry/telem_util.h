#pragma once 
#include <string>

namespace foxtrot
{
  class Client;
  class TelemetryServer;
}

void configure_telemetry_server(const std::string& fname, foxtrot::Client& cl, foxtrot::TelemetryServer& serv);
