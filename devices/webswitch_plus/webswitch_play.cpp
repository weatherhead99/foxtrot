#include <iostream>
#include "webswitch_plus.h"
#include "curlRequest.h"
#include <memory>

using std::cout;
using std::endl;

int main(int, char**)
{
    std::shared_ptr<foxtrot::protocols::CurlRequest> proto(
        new foxtrot::protocols::CurlRequest());
    
    foxtrot::devices::webswitch_plus webswitch(proto, "192.168.2.1");
    
    
    cout << "getting status of relay 1: " << webswitch.GetRelay(1) << endl;
    cout << "getting status of relay 2: " << webswitch.GetRelay(2) << endl;
    
    cout << "setting both relays to ON";
    webswitch.SetRelay(1,true);
    webswitch.SetRelay(2,true);
    
    
    
    
};
