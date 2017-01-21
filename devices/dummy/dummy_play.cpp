#include "dummyDevice.h"
#include <iostream>

#include <rttr/type>
#include <rttr/registration>

using namespace rttr;

int main(int argc, char** argv)
{
    foxtrot::devices::dummyDevice dev;
    
    std::cout << "100 counters:" << std::endl;
    for(int i =0 ; i < 100; i++)
    {
        std::cout <<  dev.getCounter() << " ";
    }
    std::cout << std::endl;
    
    std::cout << type::get(dev).get_name() << std::endl;
    
    auto resetmeth = type::get(dev).get_method("resetCounter");
    resetmeth.invoke(dev);
    
    
    std::cout << "100 counters:" << std::endl;
    for(int i =0 ; i < 100; i++)
    {
        std::cout <<  dev.getCounter() << " ";
    }
    std::cout << std::endl;
    
    
};

