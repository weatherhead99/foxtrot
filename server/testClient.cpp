#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"
#include <iostream>
#include <string>
#include <memory>

using std::cout;
using std::endl;
using namespace foxtrot;

class testClient
{
public:
    testClient(std::shared_ptr<grpc::Channel> channel) 
    : _stub(exptserve::NewStub(channel)) {};
    double randomDouble()
    {
        capability_request req;
        req.set_msgid(0);
        req.set_capname("getRandomDouble");
        req.set_devid(0);
        
        capability_response repl;
        
        grpc::ClientContext ctxt;
        cout << "invoking RPC..." << endl;
        auto status = _stub->InvokeCapability(&ctxt,req,&repl);
        
        if(status.ok())
        {
            return 0.;
        }
        else
        {
            cout << status.error_code() << ": " << status.error_message() << endl;
            return 1.;
        }
            
    }
private:
    std::unique_ptr<exptserve::Stub> _stub;
};

int main(int argc, char** argv)
{
    cout << "creating client..." << endl;
    testClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    cout << "RPC call... " << endl;
    auto repl = client.randomDouble();

    cout << "reply was.." << repl  << endl;


}
