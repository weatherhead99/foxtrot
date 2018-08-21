#include "client.h"

#include "Error.h"
#include "DeviceError.h"
#include "ProtocolError.h"

#include <algorithm>

#include <iostream>

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

foxtrot::ft_variant_printer::ft_variant_printer()
{

}

void foxtrot::ft_variant_printer::operator()(const std::string& s)
{
  _str = s;
}

void foxtrot::ft_variant_printer::operator()(const bool& b)
{
  _str = std::to_string(b);
}


void foxtrot::ft_variant_printer::operator()(const int& i)
{
  _str = std::to_string(i);

}

void foxtrot::ft_variant_printer::operator()(const double& d)
{
  _str = std::to_string(d);

}

std::string foxtrot::ft_variant_printer::string()
{
  return _str;

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

template <typename T> std::vector<T> copy_to_out_type(const std::vector<unsigned char>& in)
{
    return std::vector<T>(reinterpret_cast<const T*>(in.data()), 
                          reinterpret_cast<const T*>(in.data() + in.size()));
}


foxtrot::ft_vector_variant foxtrot::ft_variant_from_data(const foxtrot::byte_data_types& tp, const std::vector<unsigned char>& data)
{
    using namespace foxtrot;
    ft_vector_variant out;
    
    switch(tp)
    {
        case(byte_data_types::UCHAR_TYPE):
            out = data;
            break;
        case(byte_data_types::USHORT_TYPE):
            out = copy_to_out_type<unsigned short>(data);
            break;
        case(byte_data_types::UINT_TYPE):
            out = copy_to_out_type<unsigned>(data);
            break;
        case(byte_data_types::ULONG_TYPE):
            out = copy_to_out_type<unsigned long>(data);
            break;
        case(byte_data_types::SHORT_TYPE):
            out = copy_to_out_type<short>(data);
            break;
        case(byte_data_types::IINT_TYPE):
            out = copy_to_out_type<int>(data);
            break;
        case(byte_data_types::LONG_TYPE):
            out = copy_to_out_type<long>(data);
            break;
        case(byte_data_types::BFLOAT_TYPE):
            out = copy_to_out_type<float>(data);
            break;
        case(byte_data_types::BDOUBLE_TYPE):
            out =  copy_to_out_type<double>(data);
            break;
    }
    
    return out;
}



int foxtrot::find_devid_on_server(const foxtrot::servdescribe& sd, const std::string& devtp)
{
    auto device = std::find_if(sd.devs_attached().begin(), sd.devs_attached().end(),
                               [&devtp] (decltype(*sd.devs_attached().begin())& val) 
                               {
                                   if(val.second.devtype() == devtp)
                                   {
                                       return true;
                                   }
                                   else  if(val.second.devcomment() == devtp)
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



int foxtrot::get_number_of_args(const foxtrot::servdescribe& sd, int devid, int capidx)
{
  
  auto cap  = sd.devs_attached().at(devid).caps().Get(capidx);
  return cap.argnames_size();
}

int foxtrot::find_capability(const foxtrot::servdescribe& sd, int devid, const std::string& capability_name)
{
  auto caps = sd.devs_attached().at(devid).caps();
  
  auto capability_index = std::find_if(caps.begin(), caps.end(),
				       [&capability_name] (decltype(*caps.begin())& val)
				       {
					 if(val.capname() == capability_name)
					 {   
					   return true;
					 }
					 return false;
					 
					 
				       }
			       );
  
  
  if(capability_index == caps.end())
  {
    return -1;
  }
  
  return std::distance(caps.begin(), capability_index);
  
}

int foxtrot::get_arg_position(const foxtrot::servdescribe& sd, int devid, int capidx, const std::string& arg_name)
{
  auto argnames = sd.devs_attached().at(devid).caps().Get(capidx).argnames();
  
  auto arg_idx = std::find_if(argnames.begin(), argnames.end(),
			      [&arg_name] (const std::string& val)
			      {
				if(val == arg_name)
				{
				  return true;
				}
				return false;
			      }
		      );
  
  if(arg_idx == argnames.end())
  {
    return -1;
  };
  
  return std::distance(argnames.begin(),arg_idx);

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
  //TODO: optimize?
  
  ft_variant_printer pt;
  for(auto& arg: args)
  {
   boost::apply_visitor(pt,arg);  
   _lg.Trace("arg: "  + pt.string());
    
  }
  
  std::vector<ft_variant> argvec(args.begin(), args.end());
  
  return InvokeCapability(devid,capname, argvec);
  

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



