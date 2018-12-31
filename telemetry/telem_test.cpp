#include "TelemetryServer.h"
#include "NanomsgTransport.h"
#include <iostream>
#include <backward.hpp>

#include "client.h"

#include <memory>

using std::cout;
using std::endl;


foxtrot::ft_variant counter_telem(foxtrot::Client& cl)
{
  return cl.InvokeCapability(0,"getCounter"); 
};


foxtrot::ft_variant rdb_telem(foxtrot::Client& cl)
{
    return cl.InvokeCapability(0,"getRandomDouble");
};

int main(int argc, char** argv)
{
//     backward::SignalHandling sh;
    
    foxtrot::Client cl("localhost:50051");
    
    
    auto transport = std::make_unique<foxtrot::NanomsgTransport>("test_telem");
    transport->BindSocket("tcp://127.0.0.1:50052");
    
    foxtrot::TelemetryServer telem(cl,std::move(transport),100);
    
    telem.AddTelemetryItem(counter_telem,5,"counter");
    telem.AddTelemetryItem(rdb_telem,7,"randomdouble");
    

    
    auto fut = telem.runserver();
    fut.wait();
    
    
    
};
