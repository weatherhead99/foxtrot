#include "curlRequest.h"

#include <iostream>

using namespace foxtrot::protocols;

int main(int, char**)
{
    CurlRequest proto;
    
    std::cout << "starting request" << std::endl;
    auto out = proto.blocking_get_request("http://localhost:60000/state.xml");
//     auto out = proto.blocking_get_request("https://www.google.com");
    
    std::cout << "request finished..." << std::endl;
    
    std::cout << "received data: " << out << std::endl;
    
}
