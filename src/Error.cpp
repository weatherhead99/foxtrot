#include "Error.h"
#include "Logging.h"

foxtrot::Error::Error(const std::string& msg): runtime_error(msg)
{

}

foxtrot::Error::Error(const std::string& msg, Logging& lg)
: runtime_error(msg)
{
    lg.strm(sl::error) << msg;
    
    
};
