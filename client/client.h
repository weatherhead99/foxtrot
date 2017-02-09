#pragma once
#include <string>
#include <memory>
#include <iterator>
#include <iostream>

#include <type_traits>

#include <boost/variant.hpp>
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"


namespace foxtrot
{
    
    typedef boost::variant<double,int,bool,std::string> ft_variant;
    
    
    class ft_variant_visitor : public boost::static_visitor<>
    {
    public:
        ft_variant_visitor(capability_argument& arg);
        
      void operator()(double& i) const;
      void operator()(int& d) const;
      void operator()(bool& i) const;
      void operator()(const std::string& s) const;
          
        
    private:
        capability_argument& _arg;
        
    };
    
    ft_variant ft_variant_from_response(const capability_response& repl);
    
    class Client
    {
    public:
        Client(const std::string& connstr);
        ~Client();
        servdescribe DescribeServer();
        template<typename iteratortp> capability_response InvokeCapability(int devid,const std::string& capname, iteratortp begin_args, iteratortp end_args);
        
        capability_response InvokeCapability(int devid, const std::string& capname);
        
        
    private:
        std::unique_ptr<exptserve::Stub> _stub;
        std::shared_ptr<grpc::Channel> _channel;
        int _msgid = 0;
 
    };
    
    template<typename iteratortp> capability_response Client::InvokeCapability(int devid,const std::string& capname, iteratortp begin_args, iteratortp end_args)
    {
        
        static_assert(std::is_same<typename std::iterator_traits<iteratortp>::value_type, ft_variant>::value,
                           "iterator type must dereference to ft_variant");
        
        capability_request req;
        capability_response repl;
        
        req.set_msgid(_msgid++);
        req.set_devid(devid);
        req.set_capname(capname);
        
        
        int i =0;
        for(auto it = begin_args; it != end_args; it++)
        {
            capability_argument arg;
            boost::apply_visitor(ft_variant_visitor(arg),*it);
            arg.set_position(i++);
        };
        
        grpc::ClientContext ctxt;
        auto status = _stub->InvokeCapability(&ctxt,req,&repl);
        
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
    
    
}

