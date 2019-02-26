#include "curlRequest.h"
#include <backward.hpp>
#include <string>
#include <iostream>

using namespace foxtrot::protocols;

using std::cout;
using std::endl;
using std::string;

int main(int, char**)
{
    backward::SignalHandling sh;
    CurlRequest proto;
    
    string token{"REPLACE_ME"};
    
    cout << "user request to pushbullet" << endl;
    
    std::vector<string> header { "Access-Token: " +token};
    auto repl = proto.blocking_get_request("https://api.pushbullet.com/v2/users/me",
                                           &header);
    
    cout << "reply: " << repl << endl;
    
    string postreq = "{ \"body\" : \"test push\", \"title\" : \"test title\" }";
    std::vector<string> posthdr { header[0], "Content-type: application/json"};
    
    repl = proto.blocking_post_request("https://api.pushbullet.com/v2/pushes",
                                       postreq, & posthdr);
    
    
    cout << "reply: " << repl << endl;
    
    
}
