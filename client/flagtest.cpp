#include <iostream>
#include <backward.hpp>
#include "client.h"
#include "Logging.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    foxtrot::setLogFilterLevel(sl::debug);
    foxtrot::setDefaultSink();
    foxtrot::Client cl("localhost:50051");
    
    auto flags = cl.get_flag_names();
    
    cout << "flag names: " ;
    for(auto& flagname : flags)
    {
        cout << flagname << ",";
    }
    cout << endl;
    
    cout <<  "setting test flag to true" << endl;
    cl.set_server_flag("test",true);
    
    flags = cl.get_flag_names();
    
    cout << "flag names: " ;
    for(auto& flagname : flags)
    {
        cout << flagname << ",";
    }
    cout << endl;
    
    auto flagval = cl.get_server_flag("test");
    cout << "flag type: " << flagval.type().name() << endl;
    
    cout << "reading flag value: " << boost::get<bool>(flagval) << endl;
    
    
    cout << "dropping flag" << endl;
    cl.drop_server_flag("test");

    
    


}
