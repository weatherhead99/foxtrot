#include "Logging.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <iostream>

#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

//#include <boost/core/null_deleter.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>

#include <termcolor.hpp>

namespace logging = boost::log;

BOOST_LOG_GLOBAL_LOGGER_INIT(foxtrot_general_logging,logtp)
{
  logtp lg;
  return lg;
    
}

void foxtrot::setLogFilterLevel(sl level)
{
    logging::core::get()->set_filter(
        
        logging::trivial::severity >= level
                         );
    
}


void foxtrot::formatter(const logging::record_view& rec, logging::formatting_ostream& strm)
{
    namespace attr = boost::log::attributes;
    namespace expr = boost::log::expressions;
    
    auto has_color = termcolor::_internal::is_colorized(strm.stream());
    if(!has_color)
    {
        strm.stream() << termcolor::colorize;
    }
    
    sl level = logging::extract<sl>("Severity",rec).get();
    switch(level)
    {
        case(sl::fatal):
            strm.stream() << termcolor::red << termcolor::on_yellow;
            break;
        case(sl::error):
            strm.stream() << termcolor::on_red;
            break;
        case(sl::warning):
            strm.stream() << termcolor::yellow;
            break;
//         case(sl::info):
//             strm.stream() << termcolor::green;
//             break;
        case(sl::debug):
            strm.stream() << termcolor::cyan;
            break;
    }
    
    
    
    strm << "(" << logging::extract<boost::posix_time::ptime>("TimeStamp",rec) << ") " 
         << "<" << logging::extract<std::string>("Channel",rec) << "> "
         << "[" << level << "] "
         << "[" << logging::extract<attr::current_thread_id::value_type>("ThreadID",rec) << "] "
         << rec[expr::message];
    strm.stream() << termcolor::reset;
    
}


void foxtrot::setDefaultSink()
{
    namespace src = boost::log::sources;
    namespace sinks = boost::log::sinks;
    namespace expr = boost::log::expressions;
    namespace keywords = boost::log::keywords;
    namespace attr = boost::log::attributes;
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    
    auto sink = boost::make_shared<text_sink>();
    
//     boost::shared_ptr< std::ostream > stream(&std::clog, boost::null_deleter());
    
    boost::shared_ptr<std::ostream> stream(&std::clog, [] (decltype(&std::clog) lg) {});
    
    sink->locked_backend()->add_stream(stream);
    
    logging::core::get()->add_sink(sink);
    
    logging::add_common_attributes();
    
    
    sink->set_formatter(&formatter);
    
//     sink->set_formatter(
//         expr::stream << colourswitch(logging::trivial::severity)
//         << "(" << expr::attr<boost::posix_time::ptime>("TimeStamp") << ") " 
//         << "<" << expr::attr<std::string>("Channel") << "> "
//         << "[" << logging::trivial::severity  << "] "
//         << "[" << expr::attr<attr::current_thread_id::value_type>("ThreadID") << "] "
//         << expr::message << termcolor::reset
//           );

    
}





foxtrot::Logging::Logging(const std::string& channelname)
: _channelname(channelname)
{
        
};

void foxtrot::Logging::setLogChannel(const std::string& chan)
{
    _channelname = chan;
}


void foxtrot::Logging::Trace(const std::string& message)
{
    GeneralStreamRecord(message,sl::trace);
}

void foxtrot::Logging::Debug(const std::string& message)
{
    GeneralStreamRecord(message,sl::debug);
}

void foxtrot::Logging::Info(const std::string& message)
{
    GeneralStreamRecord(message,sl::info);
}

void foxtrot::Logging::Warning(const std::string& message)
{
    GeneralStreamRecord(message,sl::warning);
}


void foxtrot::Logging::Error(const std::string& message)
{
    GeneralStreamRecord(message,sl::error);
}

void foxtrot::Logging::Fatal(const std::string& message)
{
    GeneralStreamRecord(message,sl::fatal);
}


foxtrot::Logging::streamLogging foxtrot::Logging::strm(sl level)
{
  return streamLogging(*this,level);  
}

foxtrot::Logging::streamLogging::~streamLogging()
{
  _lg.GeneralStreamRecord(_oss.str(),_level);

}

foxtrot::Logging::streamLogging::streamLogging(foxtrot::Logging& lg, sl level)
  : _lg(lg), _level(level)
{
} 

foxtrot::Logging::streamLogging::streamLogging(const foxtrot::Logging::streamLogging& other)
  : _lg(other._lg)
{
  _level = other._level;
  
}


