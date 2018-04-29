#include "WatchDogEvent.h"

foxtrot::WatchDogEvent::WatchDogEvent(const std::string& eventname, unsigned int tick_check)
: eventname_(eventname), lg_(eventname)
{
}

