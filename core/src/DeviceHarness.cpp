#include <asm-generic/errno.h>
#include <exception>
#include <iostream>
#include <mutex>
#include <utility>
#include <algorithm>
#include <sstream>

#include <rttr/type>

#include <foxtrot/ContentionError.h>
#include <foxtrot/DeviceHarness.h>
//#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/typeUtil.h>



using std::cout;
using std::endl;

using namespace foxtrot;
using namespace rttr;


std::shared_ptr<foxtrot::DeviceHarness> foxtrot::DeviceHarness::create()
{
  return std::shared_ptr<DeviceHarness>(new foxtrot::DeviceHarness());
}


std::shared_ptr<foxtrot::DeviceHarness> foxtrot::DeviceHarness::ptr()
{
  return shared_from_this();
}



foxtrot::DeviceHarness::DeviceHarness() : _lg("DeviceHarness")
{
    
    
}


int foxtrot::DeviceHarness::AddDevice(std::unique_ptr<Device, void(*)(Device*)> dev)
{
    
    auto thisid = _id++;

    if(!dev->hasLockImplementation())
    {
        _devmutexes.emplace(std::piecewise_construct, std::make_tuple(thisid), std::make_tuple());
    }


    //NOTE: call this in case devices haven't in their constructor (a bit messy)
    dev->load_capability_map(true);
    
    _devvec.push_back(std::move(dev));

    
    

    
    return thisid;
    
}

int foxtrot::DeviceHarness::AddDevice(std::unique_ptr<Device> dev)
{
    
    auto raw_ptr = dev.release();

    if(raw_ptr == nullptr)
      throw std::logic_error("raw_ptr is nullptr");
    
    //TODO: some way of doing this with std::default_delete
    auto newptr = std::unique_ptr<Device,void(*)(Device*)>
    (raw_ptr,[](Device* dev) {delete dev;});
    
   
//     (raw_ptr,[](Device* dev) {});
    
    return AddDevice(std::move(newptr));
}

void foxtrot::DeviceHarness::ClearDevices(unsigned contention_timeout_ms)
{
    _lg.Info("clearuing all devices");
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


void foxtrot::DeviceHarness::RemoveDevice(int id)
{

  _lg.strm(sl::debug) << "size of device vector: " << _devvec.size();
  
  if( !_devvec[id]->hasLockImplementation())
    {
      _devvec.erase(_devvec.begin() + id);
      _devmutexes.erase(id);
    }
  else
    {
      {
	std::unique_lock lck(_devmutexes.at(id));
	_devvec.erase(_devvec.begin() + id);
      }
      _devmutexes.erase(id);
    }

  _lg.strm(sl::debug) << "size of device vector after remove: " << _devvec.size();
};

Device* const foxtrot::DeviceHarness::GetDevice(int id)
{
//     auto& devptr = _devmap.at(id);
    auto& devptr = _devvec.at(id);
    return devptr.get();
    
}




std::vector<std::string> foxtrot::DeviceHarness::GetCapabilityNames(int devid)
{
    
    std::vector<std::string> out;
    
    _lg.strm(sl::trace) << "getting device...";
    auto dev = GetDevice(devid);    
    return dev->GetCapabilityNames();

}


std::unique_lock< std::timed_mutex > DeviceHarness::lock_device_contentious(int devid, unsigned int contention_timeout_ms)
{
  
  std::unique_lock<std::timed_mutex> lock;
    
  Device* dev = GetDevice(devid);
  if(!dev)
      throw std::runtime_error("invalid device id supplied!");
  if(dev->hasLockImplementation())
  {
      //NOTE: This only works for globally locked devices!
      Capability cap;
      auto optlck = dev->obtain_lock(cap);
      if(!optlck)
          throw std::logic_error("lock function returned null optional");
      else
          lock = std::move(*optlck);
  }
  else
  {
      auto& mut = _devmutexes[devid];
      lock = std::unique_lock(mut, std::defer_lock);
  }
      
  
  if(contention_timeout_ms == 0)
  {
    _lg.Debug("no timeout, locking mutex...");
    lock.lock();
  }
  
  else
  {
    _lg.Debug("have timeout, trying to lock...");
    if(!lock.try_lock_for(std::chrono::milliseconds(contention_timeout_ms)))
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



