#include "pushbullet_api.hh"
#include <iostream>
#include <chrono>
#include <ctime>



int main(int argc, char** argv)
{
    //"o.mxtX12j2qP8fVnDJZC9hVDJ8JFsUXqFL"
//     foxtrot::pushbullet_api api("o.mxtXqP8fVnDJZC9hVDJ8JFsUXqFL");
//     auto now = std::chrono::system_clock::now();
//     std::time_t  timet = std::chrono::system_clock::to_time_t(now);
//     
//     std::cout << "the time is now: " << std::ctime(&timet) << std::endl;
//     api.push_to_channel("test push", std::string{"pushbullet_play says the time is now"}  + std::ctime(&timet), "opmdlsst");
//     
    
    
    auto token = foxtrot::authorize_pushbullet_app("zvccdV4qOqnFMISA2rriEwXatowfBKXe");
    std::cout << "API key is: " << token << std::endl;
    
}
