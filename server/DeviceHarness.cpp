#include "DeviceHarness.h"

#include <iostream>
#include <utility>
using std::cout;
using std::endl;

using namespace foxtrot;

foxtrot::DeviceHarness::DeviceHarness()
: _distribution(0,INT_MAX)
{
    
    
}


void foxtrot::DeviceHarness::AddDevice(std::unique_ptr<Device> dev)
{
    auto randid = _distribution(_generator);
    cout << "randid: " << randid << endl;
    
    _devmap.insert(std::move(std::pair<int,std::unique_ptr<Device>>{randid, std::move(dev)}));
    
}




Device* const foxtrot::DeviceHarness::GetDevice(int id)
{
    auto& devptr = _devmap.at(id);
    return devptr.get();
    
}

const std::map<int, const Device *>  foxtrot::DeviceHarness::GetDevMap() const
{
    //TODO: this is SLOOOOOOOW!
    std::map<int,const Device*> out;
    for(auto& devptrpair : _devmap)
    {
        out.insert({devptrpair.first, devptrpair.second.get()});
    }
    return out;
    
}
