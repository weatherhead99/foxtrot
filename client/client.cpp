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
    _arg.set_intarg(i);
}

void foxtrot::ft_variant_visitor::operator()(int& d) const
{
    _arg.set_dblarg(d);
}


void foxtrot::ft_variant_visitor::operator()(bool& i) const
{
    _arg.set_boolarg(i);
}

void foxtrot::ft_variant_visitor::operator()(const std::string& s) const
{
    _arg.set_strarg(s);
}

foxtrot::ft_variant foxtrot::ft_variant_from_response(const foxtrot::capability_response& repl)
{
    foxtrot::ft_variant out;
    
    //error checking
    if(repl.has_err())
    {
        auto err = repl.err();
        
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
    _stub = exptserve::NewStub(_channel);
    
}


foxtrot::servdescribe foxtrot::Client::DescribeServer()
{
    servdescribe repl;
    empty req;
    
    auto status = _stub->DescribeServer(&_ctxt, req,&repl);
    
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
