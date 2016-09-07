#include "Error.h"

foxtrot::Error::Error(const std::string& msg): runtime_error(msg)
{

}
