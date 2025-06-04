#pragma once

#include "SerialProtocol.h"
#include <string>
#include <foxtrot/Logging.h>
#include <foxtrot/protocols/SerialProtocol.h>
#include <boost/asio/any_io_executor.hpp>
#include <optional>

using namespace foxtrot;

namespace foxtrot {
  
  namespace protocols
  {
    using std::optional;
    using std::nullopt;
    
    class simpleTCPBase : public SerialProtocol
    {
    public:
      virtual unsigned bytes_available() = 0;
      simpleTCPBase(const parameterset* const instance_parameters);
      template<typename... Ts>
      simpleTCPBase(Ts&& ...pargs)
	: SerialProtocol(std::forward<Ts>(pargs)...) {};
      

    };

    
    class  simpleTCPLegacy : public simpleTCPBase
    {
    public:
      
      simpleTCPLegacy(const parameterset*const instance_parameters);
    
      virtual ~simpleTCPLegacy();
    
      void Init(const parameterset*const class_parameters) override;
      void Init(const unsigned port, const std::string& addr);
    
      virtual void open() override;
      virtual void close() override;

  
      std::string read(unsigned int len, unsigned* actlen = nullptr) override;
      void write(const std::string& data) override;
    
      std::string read_until_endl(char endlchar = '\n') override;
      
      void setchunk_size(unsigned chunk);
      unsigned getchunk_size();
    
      unsigned bytes_available() override;
    
      static bool verify_instance_parameters(const parameterset& instance_parameters);
      static bool verify_class_parameters(const parameterset& class_parameters);
    
    private:
      int _chunk_size = 1024;
      int _port;
      std::string _addr;
    
      int _sockfd;
    
      foxtrot::Logging _lg;
    
    }; 

    namespace detail
    {
      struct simpleTCPasioImpl; 
    }

    class simpleTCPasio : public simpleTCPBase
    {
    public:
      simpleTCPasio(const parameterset* const instance_parameters, optional<boost::asio::any_io_executor> exec=nullopt);
      simpleTCPasio(const string*  addr=nullptr, optional<unsigned> port=nullopt,
		    opttimeout timeout=nullopt,
		    optional<boost::asio::any_io_executor> exec=nullopt);
      virtual ~simpleTCPasio();

      void Init(const parameterset* const class_parameters) override;
      void Init(const string* const addr, optional<unsigned> port=nullopt,
		opttimeout timeout=nullopt);
      void Init();

      void open() override;
      void close() override;

      //blocking interface (traditional)
      std::string read(unsigned int len, unsigned* actlen=nullptr) override;
      void write(const std::string& data) override;

      std::string read_until_endl(char endlchar = '\n') override;
      unsigned bytes_available() override;
    private:
      std::unique_ptr<detail::simpleTCPasioImpl> pimpl;

    };


    using simpleTCP = simpleTCPLegacy;

    
}; //namespace protocols

}//namespace foxtrot
