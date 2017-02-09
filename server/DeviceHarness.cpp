#include "DeviceHarness.h"

#include <iostream>
#include <utility>
#include <algorithm>


using std::cout;
using std::endl;

using namespace foxtrot;
using namespace rttr;


foxtrot::value_types foxtrot::get_appropriate_wire_type(const rttr::type& tp)
{
    
    if(tp == type::get<void>())
    {
        return value_types::VOID;
    }
    
    if(!tp.is_arithmetic())
    {
        return value_types::STRING;
    }
    
    //check for bool
    if(tp == type::get<bool>())
    {
        return value_types::BOOL;
    }
    
    //check for float
    if( (tp == type::get<double>()) || (tp == type::get<float>())) 
    {
        return value_types::FLOAT;
    }
    
    return value_types::INT;
    
    
}


foxtrot::DeviceHarness::DeviceHarness()
{
    
    
}


void foxtrot::DeviceHarness::AddDevice(std::unique_ptr<Device, void(*)(Device*)> dev)
{
    
    auto thisid = _id++;
    _devvec.push_back(std::move(dev));
    
    _devmutexes.emplace_back();
       
}

void foxtrot::DeviceHarness::AddDevice(std::unique_ptr<Device> dev)
{
    
    auto raw_ptr = dev.release();
    
    //TODO: some way of doing this with std::default_delete
    auto newptr = std::unique_ptr<Device,void(*)(Device*)>
    (raw_ptr,[](Device* dev) {delete dev;});
    
    AddDevice(std::move(newptr));
}



const Device* const foxtrot::DeviceHarness::GetDevice(int id)
{
//     auto& devptr = _devmap.at(id);
    auto& devptr = _devvec.at(id);
    return devptr.get();
    
}

std::mutex & foxtrot::DeviceHarness::GetMutex(int id)
{
    return _devmutexes[id];
}



std::vector<std::string> foxtrot::DeviceHarness::GetCapabilityNames(int devid)
{
    std::vector<std::string> out;
    
    cout << "getting device..." << endl;
    auto dev = GetDevice(devid);
    auto tp = type::get(*dev);
    cout << "tp: " << tp.get_name() << endl;
    
    auto props = tp.get_properties();
    auto meths = tp.get_methods();
    
    out.reserve(props.size() + meths.size());
    
    for(auto& prop : props)
    {
     out.push_back(prop.get_name());   
    }
    
    
    for(auto& meth: meths)
    {
        out.push_back(meth.get_name());
    };
    
    return out;
    
}

foxtrot::devcapability foxtrot::DeviceHarness::GetDeviceCapability(int devid, const std::string& capname)
{
    auto dev = GetDevice(devid);
    auto reflecttp = type::get(*dev);
    
    //either it's a property or a method
    
    auto prop = reflecttp.get_property(capname.c_str());
    auto meth = reflecttp.get_method(capname.c_str());
    
    devcapability cap;
    cap.set_capname(capname);
    
    
    if(prop)
    {
     
      if(prop.is_readonly())
      {
            
          cap.set_tp(capability_types::VALUE_READONLY);
          cout << cap.tp() << endl;   
      }
      else
      {
          cap.set_tp(capability_types::VALUE_READWRITE);
      }
      
      cout << "rettp: " << prop.get_type().get_name() << endl;
      foxtrot::value_types rettp = get_appropriate_wire_type(prop.get_type());
      cout << "rettp wire type : " << rettp << endl;
      
      cap.set_rettp(rettp);
      
      
    }
    else if (meth)
    {
            cap.set_tp(capability_types::ACTION);
            
            //in this case, set argument names and types
            auto args =  meth.get_parameter_infos();
            
            auto rettp = meth.get_return_type();
            cap.set_rettp(get_appropriate_wire_type(rettp));
            
            
            for(auto& arg : args)
            {
                cap.add_argnames(arg.get_name());
                
                auto argtp = arg.get_type();
                
                //get a wire type
                auto wire_type = get_appropriate_wire_type(argtp);
                cap.add_argtypes(wire_type);
                
            }
            
    }
    else
    {
        throw std::out_of_range("requested capability doesn't seem to exist!");
        
    }
    
    
    return cap;
    
}





const std::map<int, const Device *>  foxtrot::DeviceHarness::GetDevMap() const
{
    //TODO: this is SLOOOOOOOW!
    std::map<int,const Device*> out;
    
    int i =0;
    for(auto& dev : _devvec)
    {
        out.insert(std::pair<int,const Device*>(i++, dev.get()));
    }
    
    return out;
    
}
