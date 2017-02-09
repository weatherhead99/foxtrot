#include "client.h"



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
