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
    
    foxtrot::Client client("localhost::50051");
    
    auto repl = client.DescribeServer();
    

    
    

}
