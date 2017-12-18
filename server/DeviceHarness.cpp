#include "DeviceHarness.h"
#include "ServerUtil.h"
#include "ContentionError.h"

#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>
#include <rttr/type>


using std::cout;
using std::endl;

using namespace foxtrot;
using namespace rttr;




foxtrot::DeviceHarness::DeviceHarness() : _lg("DeviceHarness")
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
    
    

//     auto newptr = std::unique_ptr<Device,void(*)(Device*)>
//     (raw_ptr,[](Device* dev) {});
    
    AddDevice(std::move(newptr));
}

void foxtrot::DeviceHarness::ClearDevices(unsigned contention_timeout_ms)
{
    _lg.Info("clearing all devices");
    _lg.Debug("locking all devices");
    std::vector<std::unique_lock<std::timed_mutex>> locks;
    for(int id=0; id < _devvec.size(); id++)
    {
        locks.push_back(lock_device_contentious(id,contention_timeout_ms));
        
    }
    _lg.Debug("all devices locked");
    
    _devvec.clear();
    _lg.Debug("all devices cleared");
    
}


Device* const foxtrot::DeviceHarness::GetDevice(int id)
{
//     auto& devptr = _devmap.at(id);
    auto& devptr = _devvec.at(id);
    return devptr.get();
    
}

std::timed_mutex & foxtrot::DeviceHarness::GetMutex(int id)
{
    return _devmutexes[id];
}



std::vector<std::string> foxtrot::DeviceHarness::GetCapabilityNames(int devid)
{
    std::vector<std::string> out;
    
    _lg.Info("getting device...");
    auto dev = GetDevice(devid);
    auto tp = rttr::type::get(*dev);
    _lg.Info( "tp: " + tp.get_name() );
    
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
      
      if(prop.get_type().is_array())
      {
        cap.set_tp(capability_types::STREAM);
	
      }
      
      if(prop.is_readonly())
      {
            
          cap.set_tp(capability_types::VALUE_READONLY);
          std::ostringstream oss;
          oss << cap.tp();
          _lg.Debug(oss.str() );
      }
      else
      {
          cap.set_tp(capability_types::VALUE_READWRITE);
      }
      
      _lg.Debug( "rettp: " + prop.get_type().get_name() );
      foxtrot::value_types rettp = get_appropriate_wire_type(prop.get_type());
      _lg.Debug( "rettp wire type : " + rettp );
      
      cap.set_rettp(rettp);
      
      
    }
    else if (meth)
    {
	  if(meth.get_return_type().is_array())
	  {
	   cap.set_tp(capability_types::STREAM);
	   
	  }
	  else{
      
            cap.set_tp(capability_types::ACTION);
	  }
            
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





variant DeviceHarness::call_capability(int devid, property& prop, unsigned int contention_timeout_ms)
{
  if(!prop.is_readonly())
  {
    throw std::logic_error("not a readonly property");
  };
  
  auto lock = lock_device_contentious(devid,contention_timeout_ms);
  
  auto dev = GetDevice(devid);
  auto retval = prop.get_value(*dev);
      
  return retval;
  

}

std::unique_lock< std::timed_mutex > DeviceHarness::lock_device_contentious(int devid, unsigned int contention_timeout_ms)
{
  auto& mut = GetMutex(devid);
  std::unique_lock<std::remove_reference<decltype(mut)>::type> lock(mut,std::defer_lock);
  
  if(contention_timeout_ms == 0)
  {
    _lg.Debug("no timeout, locking mutex...");
    lock.lock();
  }
  
  else
  {
    _lg.Debug("have timeout, trying to lock...");
    lock.try_lock_for(std::chrono::milliseconds(contention_timeout_ms));
    if(!lock.owns_lock())
    {
       throw ContentionError("couldn't lock device with id:" + std::to_string(devid));
    }
  
  
  }

  return lock;
  
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

prop_or_meth getCapability(Device* dev, const std::string& capname)
{
  auto devtp = rttr::type::get(*dev);
  
  auto prop = devtp.get_property(capname.c_str());
  auto meth = devtp.get_method(capname.c_str());

  if(!(meth || prop) )
  { 
    throw std::out_of_range("no matching property or method");
  }
  else if (meth.is_valid() && prop.is_valid())
  {
    throw std::logic_error("both method and property matched!");
  }
  
  if(meth.is_valid())
  {
    return meth;
  }
  
  if(prop.is_valid())
  {
    return prop;
  }
  
  throw std::logic_error("method or property not valid");
  
}



