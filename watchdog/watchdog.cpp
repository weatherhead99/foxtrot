#include "WatchDogEvent.h"
#include <Logging.h>
#include <client/client.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <boost/variant.hpp>

using std::cout;
using std::endl;

class DummyEvent : public foxtrot::WatchDogEvent
{
public:
    
    DummyEvent()
    : WatchDogEvent("dummy_event", 20)
    {
        
    }
    
    bool action(foxtrot::Client& cl) override
    {
        
        
    };
    
    bool check_trigger(foxtrot::Client& cl) override
    {
        
        auto dbl = cl.InvokeCapability(devid_,"getRandomDouble");
        cout << "double: " << dbl << endl;
        
        if(boost::get<double>(dbl) > 0 )
            return true;
        return false;
        
    };
    
    void setup_devices(const foxtrot::servdescribe& desc) override
    {
        devid_ = foxtrot::find_devid_on_server(desc, "dummyDevice");
        cout << "devid: " << devid_ << endl;
        
    };
    
    
private:
    int devid_;
    
};



int main(int argc, char** argv)
{
    foxtrot::Logging lg("watchdog");
    foxtrot::setDefaultSink();
    
    lg.Info("watchdog starting up");
    
    foxtrot::Client cl("localhost:50051");

    
    auto desc = cl.DescribeServer();
    
    
    auto de = DummyEvent();
    de.setup_devices(desc);
    
    while(true) 
    {
        lg.Trace("tick...");
        if(de.check_trigger(cl))
        {
            lg.Info("triggered!");
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        
    }


}
