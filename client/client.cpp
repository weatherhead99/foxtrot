#include "client.h"

#include "Error.h"
#include "DeviceError.h"
#include "ProtocolError.h"

foxtrot::ft_variant_visitor::ft_variant_visitor(foxtrot::capability_argument& arg) 
: _arg(arg)
{
}

void foxtrot::ft_variant_visitor::operator()(double& i) const
{
//   std::cout << "arg value at visitor is: <double> " << i << std::endl;
    _arg.set_dblarg(i);
}

void foxtrot::ft_variant_visitor::operator()(int& d) const
{
//     std::cout << "arg value at visitor is: <int> " << d << std::endl;
    _arg.set_intarg(d);
}


void foxtrot::ft_variant_visitor::operator()(bool& i) const
{
    _arg.set_boolarg(i);
}

void foxtrot::ft_variant_visitor::operator()(const std::string& s) const
{
//   std::cout << "arg value at visitor is: <string> " << s << std::endl;
    _arg.set_strarg(s);
}

foxtrot::ft_variant foxtrot::ft_variant_from_response(const foxtrot::capability_response& repl)
{
    foxtrot::ft_variant out;
    
    //error checking
    if(repl.has_err())
    {
        std::cout << "there's an error in this response..." << std::endl;
        
        std::cout << "error type: " << repl.err().tp() << std::endl;
        
        auto err = repl.err();
        
        std::cout << "constructing exceptions" << std::endl;
        
//         std::string msg = err.
        
        class foxtrot::Error except(err.msg());
        class foxtrot::DeviceError exceptdev(err.msg());
        class foxtrot::ProtocolError exceptproto(err.msg());
        
        switch(err.tp())
        {
            case(error_types::Error):
                throw except;
            case(error_types::DeviceError):
                throw exceptdev;
            case(error_types::ProtocolError):
                throw exceptproto;
            case(error_types::out_of_range):
                throw std::out_of_range(err.msg());
                
            default:
                throw std::runtime_error(err.msg());
                
        }
        
    }
    
    
    auto rettp = repl.return_case();
    switch(rettp)
    {
        case(capability_response::ReturnCase::kDblret):
            std::cout << "double" << std::endl;
            out = repl.dblret();
            break;
        case(capability_response::ReturnCase::kIntret):
            out = repl.intret();
            break;
        case(capability_response::ReturnCase::kBoolret):
            out = repl.boolret();
            break;
        case(capability_response::ReturnCase::kStringret):
            out = repl.stringret();
            break;
    }
    
    return out;
    
}





foxtrot::Client::Client(const std::string& connstr)
{
    _channel = grpc::CreateChannel(connstr,grpc::InsecureChannelCredentials());
    
    std::cout << "connect status: " << _channel->GetState(true) << std::endl;
    _stub = exptserve::NewStub(_channel);
    
}

foxtrot::Client::~Client()
{
 
}


foxtrot::servdescribe foxtrot::Client::DescribeServer()
{
    servdescribe repl;
    empty req;
    
    std::cout << "invoking describe RPC..." << std::endl;
    
    grpc::ClientContext ctxt;
    auto status = _stub->DescribeServer(&ctxt, req,&repl);
    
    if(status.ok())
    {
     return repl;      
    }
    else
    {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        throw std::runtime_error("GRPC Error");
    
    }
    
}

foxtrot::ft_variant foxtrot::Client::InvokeCapability(int devid, const std::string& capname)
{
    std::vector<ft_variant> empty;
    return InvokeCapability(devid,capname,empty.begin(),empty.end());
    
}

