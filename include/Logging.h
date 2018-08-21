#pragma once

#ifndef FOXTROT_CONAN_BUILD
#define BOOST_LOG_DYN_LINK
#endif

#include "foxtrot_export.h"

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <string>
#include <iostream>
#include <sstream>

using boost::log::sources::severity_logger;
using boost::log::sources::severity_channel_logger;
using sl = boost::log::trivial::severity_level;
namespace logging = boost::log;


typedef severity_channel_logger<sl,std::string> logtp;


BOOST_LOG_GLOBAL_LOGGER(foxtrot_general_logging, logtp);



namespace foxtrot
{
    void FOXTROT_EXPORT setLogFilterLevel(sl level);
    void FOXTROT_EXPORT setLogChannel(const std::string& chan);
    
    void FOXTROT_EXPORT formatter(const logging::record_view& rec, logging::formatting_ostream& strm);
    void FOXTROT_EXPORT setDefaultSink();
    
    
    class FOXTROT_EXPORT Logging 
    {
           
    public:
        Logging(const std::string& channelname);
        
        void Trace(const std::string& message);
        void Debug(const std::string& message);
        void Info(const std::string& message);
        void Warning(const std::string& message);
        void Error(const std::string& message);
        void Fatal(const std::string& message);
        
        void setLogChannel(const std::string& chan);
        
	class FOXTROT_EXPORT streamLogging{
	public:
	  friend class Logging;
	  template<typename T> streamLogging& operator<<(T const& value);
	  ~streamLogging();
	private:
	  streamLogging(Logging& lg, sl level);
	  streamLogging(const streamLogging& other);
	  streamLogging(streamLogging&& other);
	  Logging& _lg;
	  sl _level;
	  std::ostringstream _oss;
	};
	
	streamLogging strm(sl level);
	
    private:
        inline void GeneralStreamRecord(const std::string& message, boost::log::trivial::severity_level sev)
        {
            logtp lg(logging::keywords::channel=_channelname, 
                         logging::keywords::severity = sev);
            logging::record rec = lg.open_record();
            if(rec)
            {
            logging::record_ostream strm(rec);   
                strm << message ;
                strm.flush();
                lg.push_record(boost::move(rec));
            }
        }
        
        std::string _channelname;
        
        
    };
    
}
   
template<typename T>
foxtrot::Logging::streamLogging& foxtrot::Logging::streamLogging::operator<<(T const& value)
{
  _oss << value;
  return *this;

}
