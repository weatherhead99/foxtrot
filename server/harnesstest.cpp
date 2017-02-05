#include "dummyDevice.h"
#include <iostream>
#include <memory>
#include "DeviceHarness.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    
    std::unique_ptr<foxtrot::devices::dummyDevice> dev(new foxtrot::devices::dummyDevice);
    
    cout << "construct harness" << endl;
    foxtrot::DeviceHarness harness;
    
    cout << "adding device.. " << endl;
    harness.AddDevice(std::move(dev));
    
    auto devmap = harness.GetDevMap();
    for(auto& item : devmap)
    {
        
        cout << "id: " << item.first << endl;
    };
    
    
    cout << "getting caps.." << endl;    
    auto capnames = harness.GetCapabilityNames(0);
    
    for(auto& cap : capnames)
    {
        cout << "capability name: " << cap << endl;
    }
    
    
    
    auto rttr_float = rttr::type::get<double>();
    
    auto val = rttr_float.create({1.2});
    
    bool canconverttoint;
    val.to_int(&canconverttoint);
    
    cout << canconverttoint << endl;


}
