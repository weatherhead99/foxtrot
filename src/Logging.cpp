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
    
    sink->set_formatter(
        expr::stream 
        << "(" << expr::attr<boost::posix_time::ptime>("TimeStamp") << ") " 
        << "<" << expr::attr<std::string>("Channel") << "> "
        << "[" << logging::trivial::severity  << "] "
        << "[" << expr::attr<attr::current_thread_id::value_type>("ThreadID") << "] "
        << expr::message
          );
    
}



foxtrot::Logging::Logging(const std::string& channelname)
: _channelname(channelname)
{
        
};


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
