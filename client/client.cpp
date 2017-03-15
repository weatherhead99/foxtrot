#include "client.h"

#include "Error.h"
#include "DeviceError.h"
#include "ProtocolError.h"

#include <algorithm>

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
//             std::cout << "double" << std::endl;
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

int foxtrot::find_devid_on_server(foxtrot::servdescribe& sd, const std::string& devtp)
{
    auto device = std::find_if(sd.devs_attached().begin(), sd.devs_attached().end(),
                               [&devtp] (decltype(*sd.devs_attached().begin())& val) 
                               {
                                   if(val.second.devtype() == devtp)
                                   {
                                       return true;
                                   }
                                   return false;
                            }
                       );
  if (device == sd.devs_attached().end())
  {
    
    return -1;
  }

  return device->first;
  

}




foxtrot::Client::Client(const std::string& connstr)
: _lg("foxtrotClient")
{
    _channel = grpc::CreateChannel(connstr,grpc::InsecureChannelCredentials());
    
    _lg.Debug("connect status: " + std::to_string(_channel->GetState(true)));
    _stub = exptserve::NewStub(_channel);
    
}

foxtrot::Client::~Client()
{
 
}


foxtrot::servdescribe foxtrot::Client::DescribeServer()
{
    servdescribe repl;
    empty req;
    
    _lg.Debug("invoking describe RPC...");
    
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


foxtrot::ft_variant foxtrot::Client::InvokeCapability(int devid, const std::string& capname, std::initializer_list< foxtrot::ft_variant > args)
{
  return InvokeCapability(devid,capname, args);
  

}

foxtrot::Client::capability_proxy::capability_proxy(foxtrot::Client& cl, int devid, const std::string& capname)
: _clientbackref(cl),_devid(devid), _capname(capname)
{

}

foxtrot::ft_variant foxtrot::Client::capability_proxy::operator()(std::initializer_list< foxtrot::ft_variant > args)
{
 return _clientbackref.InvokeCapability(_devid,_capname,args);
}


foxtrot::Client::capability_proxy foxtrot::Client::call(int devid, const std::string& capname)
{
  return capability_proxy(*this, devid,capname);
  
}
