#pragma once

#ifndef BOOST_LOG_DYN_LINK
#define BOOST_LOG_DYN_LINK
#endif

#include <string>
#include <iostream>
#include <sstream>

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>


#include <foxtrot/foxtrot_core_export.h>


using boost::log::sources::severity_logger;
using boost::log::sources::severity_channel_logger;
using sl = boost::log::trivial::severity_level;
namespace logging = boost::log;


typedef severity_channel_logger<sl,std::string> logtp;


BOOST_LOG_GLOBAL_LOGGER(foxtrot_general_logging, logtp);

#define FT_LOG_AND_THROW(lg, T, sl, msg) \
throw *(lg.streamthrow<T>(sl) << msg)

namespace foxtrot
{
    void FOXTROT_CORE_EXPORT setLogFilterLevel(sl level);
    void FOXTROT_CORE_EXPORT setLogChannel(const std::string& chan);
    
    void FOXTROT_CORE_EXPORT formatter(const logging::record_view& rec, logging::formatting_ostream& strm);
    void FOXTROT_CORE_EXPORT setDefaultSink();
    
    
    class FOXTROT_CORE_EXPORT Logging 
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
        
        void Log(const std::string& message, sl level);
        
        template<typename T>
        void logAndThrow(const std::string& message, sl level = sl::error);
        
        
	class FOXTROT_CORE_EXPORT streamLogging{
	public:
	  friend class Logging;
	  template<typename T> streamLogging& operator<<(T const& value);
	  virtual ~streamLogging();
	protected:
	  streamLogging(Logging& lg, sl level);
	  streamLogging(const streamLogging& other);
	  streamLogging(streamLogging&& other);
      bool nolog= false;
	  Logging& _lg;
	  sl _level;
	  std::ostringstream _oss;
	};
	
    template<typename T>
    struct streamThrowLogging : public streamLogging {
        friend class Logging;
        template<typename R> streamThrowLogging& operator<<(R const& value)
        {
            _oss << value;
            return *this;
        };
        T operator*();
        virtual ~streamThrowLogging() override;
        
    private:
        streamThrowLogging(Logging& lg, sl level);
        
    };
    
    
	streamLogging strm(sl level);
    template<typename T> 
    streamThrowLogging<T> strmthrow(sl level);
	
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

template<typename T>
foxtrot::Logging::streamThrowLogging<T>::streamThrowLogging(Logging& lg, sl level)
: streamLogging(lg,level)
{
    nolog = true;
};

template<typename T>
T foxtrot::Logging::streamThrowLogging<T>::operator*()
{
    return T(_oss.str());
}

template<typename T>
foxtrot::Logging::streamThrowLogging<T>::~streamThrowLogging()
{
    if(!std::uncaught_exceptions())
        _lg.logAndThrow<T>(_oss.str(), _level);
    else
    {
        nolog = false;
    }
}


template<typename T>
void foxtrot::Logging::logAndThrow(const std::string& message, sl level)
{
    Log(message,level);
    throw T(message);
}


template<typename T>
foxtrot::Logging::streamThrowLogging<T> foxtrot::Logging::strmthrow(sl level)
{
    return streamThrowLogging<T>(*this,level);
    
};

