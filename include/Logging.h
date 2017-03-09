#pragma once

#define BOOST_LOG_DYN_LINK 

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <string>


using boost::log::sources::severity_logger;
using boost::log::sources::severity_channel_logger;
using sl = boost::log::trivial::severity_level;
namespace logging = boost::log;


typedef severity_channel_logger<sl,std::string> logtp;


BOOST_LOG_GLOBAL_LOGGER(foxtrot_general_logging, logtp);



namespace foxtrot
{
    void setLogFilterLevel(sl level);
    void setLogChannel(const std::string& chan);
    
    void setDefaultSink();
    
    
    class Logging 
    {
           
    public:
        Logging(const std::string& channelname);
        
        void Trace(const std::string& message);
        void Debug(const std::string& message);
        void Info(const std::string& message);
        void Warning(const std::string& message);
        void Error(const std::string& message);
        void Fatal(const std::string& message);
        
    private:
        inline void GeneralStreamRecord(const std::string& message, boost::log::trivial::severity_level sev)
        {
            logtp lg(logging::keywords::channel=_channelname, 
                         logging::keywords::severity = sev);
            logging::record rec = lg.open_record();
            if(rec)
            {
            logging::record_ostream strm(rec);   
                strm << message;
                strm.flush();
                lg.push_record(boost::move(rec));
            }
        }
        
        std::string _channelname;
        
        
    };
    
    
    
}
