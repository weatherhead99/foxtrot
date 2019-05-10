#include "dummyDevice.h"
#include <iostream>

#include <rttr/type>
#include <rttr/registration>

#include <memory>
#include "Device.h"
#include <vector>

#include <istream>

using namespace rttr;
using std::cout;
using std::endl;

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
    
    
    
    cout << "adding two numbers.." << endl;
    
    
    std::vector<rttr::argument> args{1,15};
    
    auto addmeth = tp.get_method("add");
    
    auto ret = addmeth.invoke_variadic(dev,args);
    
    
    cout << "return type: " << ret.get_type().get_name() << endl;
    
    
};

