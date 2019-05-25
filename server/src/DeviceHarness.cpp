#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>

#include <rttr/type>

#include <foxtrot/ContentionError.h>
#include <foxtrot/server/DeviceHarness.h>
#include <foxtrot/server/ServerUtil.h>




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
    
    _lg.strm(sl::trace) << "getting device...";
    auto dev = GetDevice(devid);
    return dev->GetCapabilityNames();

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
    
    _lg.strm(sl::trace) << "getting capability for: " << capname;
    
    
    if(prop)
    {
        _lg.strm(sl::trace) << "it's a property...";
      
#ifndef NEW_RTTR_API
      if(prop.get_type().is_array())
#else
      if(prop.get_type().is_sequential_container())
#endif
      {
          _lg.strm(sl::trace) << "it's a stream...";
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
          
          auto proptp = prop.get_type();
          
#ifdef NEW_RTTR_API
          //TODO: probably much more elegant way to do this!
          cap.add_argnames(std::string(proptp.get_name()));
#else
          cap.add_argnames(proptp.get_name());
#endif
          cap.add_argtypes(get_appropriate_wire_type(proptp));
          
      }
      
      _lg.strm(sl::trace) << "rettp: " << prop.get_type().get_name();
      foxtrot::value_types rettp = get_appropriate_wire_type(prop.get_type());
      _lg.strm(sl::trace) <<  "rettp wire type : " <<  rettp ;
      
      cap.set_rettp(rettp);
      
      
    }
    else if (meth)
    {
        _lg.strm(sl::trace) << "it's a method...";
        
#ifndef NEW_RTTR_API
        _lg.strm(sl::trace) << "old rttr api in use...";
	  if(meth.get_return_type().is_array())
#else
          
     _lg.strm(sl::trace) << "return type: " << meth.get_return_type().get_name(); 
          
      if(meth.get_return_type().is_sequential_container())
#endif
	  {
        _lg.strm(sl::trace) << "it's a stream...";
	   cap.set_tp(capability_types::STREAM);
	   
	  }
	  else{
        _lg.strm(sl::trace) << "it's an action...";
            cap.set_tp(capability_types::ACTION);
	  }
            
            //in this case, set argument names and types
            auto args =  meth.get_parameter_infos();
            
            auto rettp = meth.get_return_type();
            cap.set_rettp(get_appropriate_wire_type(rettp));
            
            
            for(auto& arg : args)
            {
#ifdef NEW_RTTR_API
                cap.add_argnames(std::string(arg.get_name()));
#else
                cap.add_argnames(arg.get_name());
#endif
                
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



