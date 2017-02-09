#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include <iostream>
#include <string>
#include <memory>
#include "backward.hpp"

#include "client.h"

using std::cout;
using std::endl;
using namespace foxtrot;


int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    
    foxtrot::Client client("localhost:50051");
    
    auto repl = client.DescribeServer();
    
    cout << "server comment: " << repl.servcomment() << endl;
    
    for(auto& dev : repl.devs_attached())
    {
        cout << "id: " << dev.first <<"\t";
        cout <<"type: "<< dev.second.devtype() <<"\t";
        cout << endl;
        
        auto caps =dev.second.caps();
        for(auto& cap : caps)
        {
         cout << "capability name:" << cap.capname() << endl;   
        }
        
    };
    
    
    cout << "invoking randomdouble..." << endl;
    
    auto response = client.InvokeCapability(0,"getRandomDouble");
      
    cout << response << endl;
    
    
    for(int i =0; i< 3; i++)
    {
        auto counter = client.InvokeCapability(0,"getCounter");
        cout << "counter: " << boost::get<int>(counter) << endl;
        
    }

//     cout << "now for an exception..." << endl;
//     
//     response = client.InvokeCapability(0,"brokenMethod");
//     
    
//     cout << "an unsupported type..." << endl;
//     response = client.InvokeCapability(0,"unsupportedtype");


    cout << "adding two numbers..." << endl;
    
    std::vector<foxtrot::ft_variant> args { 12,  15   };
    
    response = client.InvokeCapability(0,"add",args.begin(), args.end());
    
    cout << "the response is: "<< boost::get<int>(response) << endl;
    
}
