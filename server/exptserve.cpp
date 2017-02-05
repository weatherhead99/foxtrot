#include "backward.hpp"
#include "ServerImpl.h"

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    foxtrot::ServerImpl serv("test server");
    serv.Run();
    


};
