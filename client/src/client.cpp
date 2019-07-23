#include <algorithm>
#include <iostream>
#include <foxtrot/Error.h>
#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>

#include <foxtrot/client/client.h>

//WARNING: HACK: THIS MUST ALL BE FIXED PROPERLY TO BE NOT FRAGILE! 
//HACK: currently only using simplevariant types etc etc

foxtrot::ft_variant_visitor::ft_variant_visitor(foxtrot::capability_argument& arg) 
: _arg(arg)
{
}

void foxtrot::ft_variant_visitor::operator()(double& i) const
{
    auto simplevar = _arg.mutable_value()->mutable_simplevar();
    simplevar->set_dblval(i);
    simplevar->set_size(sizeof(double));
}

void foxtrot::ft_variant_visitor::operator()(int& d) const
{
    auto simplevar = _arg.mutable_value()->mutable_simplevar();
    simplevar->set_intval(d);
    simplevar->set_size(sizeof(int));
}


void foxtrot::ft_variant_visitor::operator()(bool& i) const
{
    auto simplevar = _arg.mutable_value()->mutable_simplevar();
    simplevar->set_boolval(i);
    simplevar->set_size(sizeof(bool));
}

void foxtrot::ft_variant_visitor::operator()(const std::string& s) const
{
    auto simplevar = _arg.mutable_value()->mutable_simplevar();
    simplevar->set_stringval(s);
    
}


foxtrot::ft_variant_flag_visitor::ft_variant_flag_visitor(serverflag& flag)
: _flag(flag)
{    
};


void foxtrot::ft_variant_flag_visitor::operator()(const double& d) const
{
    _flag.set_dblval(d);
}

void foxtrot::ft_variant_flag_visitor::operator()(const int& i) const
{
    _flag.set_intval(i);
}

void foxtrot::ft_variant_flag_visitor::operator()(const bool& b) const
{
    _flag.set_boolval(b);
}

void foxtrot::ft_variant_flag_visitor::operator()(const std::string& s) const
{ 
    _flag.set_stringval(s);
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
    foxtrot::Logging lg("ft_variant_from_response");
    
    check_repl_err(repl,&lg);
    
    auto retval = repl.returnval();
    if(!retval.has_simplevar())
    {
        throw std::runtime_error("can't deal with non simplevar responses in c++ client yet!");
    }
    switch(retval.simplevar().value_case())
    {
        case(ft_simplevariant::ValueCase::kDblval):
            out = retval.simplevar().dblval(); break;
        case(ft_simplevariant::ValueCase::kIntval):
            out = static_cast<int>(retval.simplevar().intval()); break;
        case(ft_simplevariant::ValueCase::kUintval):
            out = static_cast<int>(retval.simplevar().uintval()); break;
        case(ft_simplevariant::ValueCase::kBoolval):
            out = retval.simplevar().boolval(); break;
        case(ft_simplevariant::ValueCase::kStringval):
            out = retval.simplevar().stringval(); break;
            
    }
    
    return out;
    

}

foxtrot::ft_variant foxtrot::ft_variant_from_response(const foxtrot::serverflag& repl)
{
    foxtrot::ft_variant out;
    foxtrot::Logging lg("ft_variant_from_response");
    check_repl_err(repl,&lg);
    
    auto rettp = repl.arg_case();
    switch(rettp)
    {
        case(serverflag::ArgCase::kDblval):
            out = repl.dblval();
            break;
        case(serverflag::ArgCase::kIntval):
            out = repl.intval();
            break;
        case(serverflag::ArgCase::kBoolval):
            out = repl.boolval();
            break;
        case(serverflag::ArgCase::kStringval):
            out = repl.stringval();
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
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message();
        throw std::runtime_error("GRPC Error");
    }
    
}


foxtrot::ft_variant foxtrot::Client::get_server_flag(const std::string& flagname)
{
    serverflag req;
    serverflag repl;
    
    req.set_msgid(_msgid++);
    req.set_flagname(flagname);
    
    grpc::ClientContext ctxt;
    auto status = _stub->GetServerFlag(&ctxt, req, &repl);
    
    if(status.ok())
    {
        check_repl_err(repl, &_lg);
        return ft_variant_from_response(repl);
    }
    else
    {
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message();
        throw std::runtime_error("GRPC Error");
    }
    
};

void foxtrot::Client::set_server_flag(const std::string& flagname, const ft_variant& val)
{
    serverflag req;
    serverflag repl;
    req.set_msgid(_msgid++);
    req.set_flagname(flagname);
    
    boost::apply_visitor(ft_variant_flag_visitor(req), val);
    
    grpc::ClientContext ctxt;
    auto status = _stub->SetServerFlag(&ctxt, req, &repl);
    
    if(status.ok())
    {
        check_repl_err(repl, &_lg);
    }
    else
    {
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message();
        throw std::runtime_error("GRPC Error");
    }
    
};

void foxtrot::Client::drop_server_flag(const std::string& flagname)
{
    serverflag req;
    serverflag repl;
    req.set_msgid(_msgid++);
    req.set_flagname(flagname);
    
    grpc::ClientContext ctxt;
    auto status = _stub->DropServerFlag(&ctxt, req, &repl);
    
    if(status.ok())
    {
        check_repl_err(repl, &_lg);
    }
    else
    {
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message();
        throw std::runtime_error("GRPC Error");
    }
    
    
};


std::vector<std::string> foxtrot::Client::get_flag_names()
{
    serverflaglist repl;
    empty req;
    
    grpc::ClientContext ctxt;
    auto status = _stub->ListServerFlags(&ctxt, req, &repl);
    
    if(status.ok())
    {
        check_repl_err(repl, &_lg);
        std::vector<std::string> out;
        auto flags = repl.flags();
        out.reserve(flags.size());
        
        for(auto& flag : flags)
          out.push_back(flag.flagname());
        
        return out;
        
    }
    else
    {
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message();
        throw std::runtime_error("GRPC Error");
    }
    
};


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

void foxtrot::Client::BroadcastNotification(const std::string& body, const std::string* title, const std::string* channel)
{
    broadcast_notification bn;
    broadcast_notification repl;
    bn.set_body(body);
    
    if(title)
    {
        bn.set_title(*title);
        bn.set_use_default_title(false);
    }
    else
        bn.set_use_default_title(true);
    
    if(channel)
    {
        bn.set_channel_target(*channel);
        bn.set_use_default_channel(false);
    }
    else
        bn.set_use_default_channel(true);
    
    grpc::ClientContext ctxt;
    auto status = _stub->BroadcastNotification(&ctxt,bn,&repl);
    
    if(status.ok())
    {
        check_repl_err(repl, &_lg);
    }
    else
    {
        _lg.strm(sl::error) << status.error_code() << ": " << status.error_message();
        throw std::runtime_error("GRPC Error");
    }
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



