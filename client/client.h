#pragma once
#include <string>
#include <memory>
#include <iterator>
#include <iostream>

#include <type_traits>
#include <initializer_list>

#include <boost/variant.hpp>
#include <grpc++/grpc++.h>
#include "foxtrot.grpc.pb.h"

#include "Logging.h"

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
    
    class ft_variant_printer : public boost::static_visitor<>
    {
    public:
      ft_variant_printer();
      
      void operator()(const double& d);
      void operator()(const int& i);
      void operator()(const bool& b);
      void operator()(const std::string& s);
      
      std::string string();
      
    private:
      std::string _str;
      
    };
    
    
    
    class Client
    {
    public:
	class capability_proxy
	{
	  friend class Client;
	  ft_variant operator()(std::initializer_list<ft_variant> args);
	protected:
	  capability_proxy(Client& cl, int devid, const std::string& capname);
	  
	private:
	  Client& _clientbackref;
	  std::string _capname;
	  int _devid;
	};
      
      
        Client(const std::string& connstr);
        ~Client();
        servdescribe DescribeServer();
	
        template<typename iteratortp> ft_variant InvokeCapability(int devid,const std::string& capname, iteratortp begin_args, iteratortp end_args);
        
        template<typename containertp> ft_variant InvokeCapability(int devid,const std::string& capname, containertp args);
        ft_variant InvokeCapability(int devid, const std::string& capname);
	
	ft_variant InvokeCapability(int devid, const std::string& capname, std::initializer_list<ft_variant> args);
	
        capability_proxy call(int devid, const std::string& capname);
        
// 	std::vector<unsigned char> FetchData(int devid, const std::string& capname, unsigned dataid, unsigned chunksize, std::initializer_list<ft_variant> args);
	template <typename iteratortp> std::vector<unsigned char> FetchData(int devid, const std::string& capname, unsigned dataid, unsigned chunksize, iteratortp begin_args, iteratortp end_args);
	
        
    private:
        template< typename reqtp, typename iteratortp> void fill_args(reqtp& req, iteratortp args_begin, iteratortp args_end)
	{
	  auto outargs = req.mutable_args();
	  int i=0;
	  for(auto it = args_begin; it != args_end; it++)
	  {
	   auto arg = outargs->Add(); 
	   boost::apply_visitor(ft_variant_visitor(*arg),*it);
	   arg->set_position(i++);
	   
	  };
	  
	}
      
        std::unique_ptr<exptserve::Stub> _stub;
        std::shared_ptr<grpc::Channel> _channel;
        int _msgid = 0;
        Logging _lg;
 
    };
    
    int find_devid_on_server(foxtrot::servdescribe& sd, const std::string& devtp);
    template<typename iteratortp> ft_variant Client::InvokeCapability(int devid,const std::string& capname, iteratortp begin_args, iteratortp end_args)
    {
        
        static_assert(std::is_same<typename std::iterator_traits<iteratortp>::value_type, ft_variant>::value,
                           "iterator type must dereference to ft_variant");
        
        capability_request req;
        capability_response repl;
        
        req.set_msgid(_msgid++);
        req.set_devid(devid);
        req.set_capname(capname);
        
	fill_args(req, begin_args,end_args);
	
        grpc::ClientContext ctxt;
        auto status = _stub->InvokeCapability(&ctxt,req,&repl);
        
        if(status.ok())
        {
            return ft_variant_from_response(repl);
        }
        else
        {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        throw std::runtime_error("GRPC Error");
        }
        
      
    }
    
    template<typename containertp> ft_variant Client::InvokeCapability(int devid,const std::string& capname, containertp args)
    {
        return InvokeCapability(devid,capname,args.begin(),args.end());
        
    };
    
    template <typename iteratortp> std::vector<unsigned char> Client::FetchData(int devid, const std::string& capname, unsigned dataid, unsigned chunksize, iteratortp begin_args, iteratortp end_args)
    {
        static_assert(std::is_same<typename std::iterator_traits<iteratortp>::value_type, ft_variant>::value,
                           "iterator type must dereference to ft_variant");
        
      chunk_request req;
      req.set_devid(devid);
      req.set_capname(capname);
      
      req.set_msgid(_msgid++);
      req.set_chunksize(chunksize);
     
      fill_args(req, begin_args, end_args);
      
      grpc::ClientContext ctxt;
      auto reader = _stub->FetchData(&ctxt,req);
      
      std::vector<unsigned char> out;
      
      datachunk repl;
      while(reader->Read(&repl))
      {
	auto thisdat = repl.data();
	out.insert(out.end(),std::begin(thisdat), std::end(thisdat));
	
	//TODO: error handling here!
      }
      
      return out;
    }

    
}

