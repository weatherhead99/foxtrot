#include <foxtrot/ProtocolError.h>

foxtrot::ProtocolError::ProtocolError(const std::string& msg): Error(msg)
{

}

foxtrot::ProtocolError::ProtocolError(const std::string& msg, Logging& lg) : Error(msg,lg)
{
    
}
