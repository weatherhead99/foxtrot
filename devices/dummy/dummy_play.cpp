#include "dummyDevice.h"
#include <iostream>

#include <rttr/type>
#include <rttr/registration>

#include <memory>
#include "Device.h"

#include <istream>

using namespace rttr;

int main(int argc, char** argv)
{
    
    std::unique_ptr<foxtrot::Device> dev(new foxtrot::devices::dummyDevice );
    
    auto dummydev = static_cast<foxtrot::devices::dummyDevice*>(dev.get());
    
    std::cout << "100 counters:" << std::endl;
    for(int i =0 ; i < 100; i++)
    {
        std::cout <<  dummydev->getCounter() << " ";
    }

    std::cout << std::endl;
    std::cout << type::get(*dev).get_name() << std::endl;
    
    auto tp = type::get(*dev);
    
    
    std::string cmd;
    
    std::cin >> cmd;
    
    
    
    auto resetmeth = tp.get_method(cmd.c_str());
    resetmeth.invoke(dev);
    
    
    std::cout << "100 counters: " << std::endl;
    for(int i =0 ; i < 100; i++)
    {
        std::cout <<  dummydev->getCounter() << " ";
    }
  
    std::cout << std::endl;
    
    
};

