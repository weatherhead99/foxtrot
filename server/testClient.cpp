#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include <iostream>
#include <string>
#include <memory>
#include "backward.hpp"

using std::cout;
using std::endl;
using namespace foxtrot;


int test_describeserver(std::shared_ptr<grpc::Channel> channel)
{
    servdescribe repl;
    empty req;
    grpc::ClientContext ctxt;
    cout << "invoking RPC..." << endl;
    std::unique_ptr<exptserve::Stub> stub(exptserve::NewStub(channel));
    
    auto status = stub->DescribeServer(&ctxt,req,&repl);
    if(status.ok())
    {
        cout << "server comment is:" << repl.servcomment() << endl;
        
        return 0;
    }
    else
    {
        cout << status.error_code() << ": " << status.error_message() << endl;
        return 1;
    }

}

int test_invokecapability(std::shared_ptr<grpc::Channel> channel)
{
    capability_request req;
    capability_response repl;
    
    grpc::ClientContext ctxt;
    cout << "invoking RPC..." << endl;
    std::unique_ptr<exptserve::Stub> stub(exptserve::NewStub(channel));
    
    auto status=  stub->InvokeCapability(&ctxt,req,&repl);
    if(status.ok())
    {
        return 0;
    }
    else
    {
        cout << status.error_code() << ": " << status.error_message() << endl;
        return 1;
    }
    
}


int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    cout << "RPC call... " << endl;
    auto repl = test_describeserver(channel);
    cout << "reply was.." << repl  << endl;
    repl = test_invokecapability(channel);
    

}
