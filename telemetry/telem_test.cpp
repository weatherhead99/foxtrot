#include "TelemetryServer.h"
#include <iostream>
#include <backward.hpp>

#include "client.h"

using std::cout;
using std::endl;


foxtrot::ft_variant counter_telem(foxtrot::Client& cl)
{
  return cl.InvokeCapability(0,"getCounter"); 
};


int main(int argc, char** argv)
{
//     backward::SignalHandling sh;
    
    foxtrot::Client cl("localhost:50051");
    
    foxtrot::TelemetryServer telem("test_telem",cl,100);
    
    telem.AddTelemetryItem(counter_telem,5,"counter");
    
    telem.BindSocket("tcp://127.0.0.1:50052");
    
    auto fut = telem.runserver();
    fut.wait();
    
};
