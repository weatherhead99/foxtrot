#include <sstream>
#include <algorithm>


#include <rttr/registration>

#include <foxtrot/Device.h>
#include <foxtrot/ReflectionError.h>


//NOTE: copied (probably missing edge cases!) from boost::hash_combine
template<typename T>
void hash_combine(std::size_t& prevhash, const T& val)
{
    auto hash = std::hash<T>{}(val);
    prevhash ^= hash  + 0x9e3779b9 + (prevhash << 6) + (prevhash >> 2);
};


bool foxtrot::Capability::operator==(const foxtrot::Capability& other)
{
    if(type != other.type)
        return false;
    if(Returntype != other.Returntype)
        return false;
    auto othertpit = other.Argtypes.cbegin();
    for(auto& tp: Argtypes)
    {
        if(tp != *(othertpit++))
            return false;
    }
    
    if(CapabilityName != other.CapabilityName)
        return false;
    
    auto otherargnmit = other.Argnames.cbegin();
    for(auto& argnm: Argnames)
    {
        if(argnm != *(otherargnmit++))
            return false;
    }
    
    return true;
};

std::size_t foxtrot::CapabilityHash::operator()(const foxtrot::Capability& cap) const noexcept
{
    auto hash = std::hash<unsigned char>{}(static_cast<unsigned char>(cap.type));
    hash_combine(hash, cap.CapabilityName);
    
    std::for_each(cap.Argnames.cbegin(), cap.Argnames.cend(), 
                  [&hash] (const std::string& s) 
                  {
                      hash_combine(hash, s);
                  });
    
    std::for_each(cap.Argtypes.cbegin(), cap.Argtypes.cend(),
                  [&hash] (const rttr::type& t)
                  {
                      //WARNING:changes each run, don't cache CapabilityHashes
                      hash_combine(hash, t.get_id());
                  });
    
    hash_combine(hash, cap.Returntype.get_id());
    
    return hash;
};


foxtrot::Device::Device(std::shared_ptr< foxtrot::CommunicationProtocol > proto, const std::string& comment, bool load_capabilities)
: _proto(std::move(proto)), _devcomment(comment), lg_("Device")
{
    if(load_capabilities)
        load_capability_map(true);
}

foxtrot::Device::~Device()
{

}


const std::string foxtrot::Device::getDeviceTypeName() const
{
    return "Device";
}


const std::string foxtrot::Device::getDeviceComment() const
{
    return _devcomment;
};
void foxtrot::Device::setDeviceComment(const std::string& comment)
{
    _devcomment = comment;
}



void sanitize_arg(rttr::variant& argin, const rttr::type& target_tp, int pos, foxtrot::Logging* lg= nullptr)
{
    if(!argin.is_valid())
        throw std::logic_error("argument variant is not valid! This should not happen");
    
    if(!argin.can_convert(target_tp))
    {
        std::ostringstream oss;
        oss << "argument at position: " << pos << "of type: " << argin.get_type().get_name() << "cannot be converted to needed type: " << target_tp.get_name();
        throw std::logic_error(oss.str());
    }

    if(argin.get_type() != target_tp)
    {
        if(lg)
            lg->strm(sl::trace) << "need to convert";
        bool success = argin.convert(target_tp);
        if(!success)
            throw foxtrot::ReflectionError("failed to convert argument at position: " + std::to_string(pos));
    };
    
    if(lg)
        lg->strm(sl::trace) << "printing prepared arg: " << argin.to_string();
    
    if(lg)
    {
        lg->strm(sl::trace) << "argument type: " << argin.get_type().get_name();
        lg->strm(sl::trace) << "target type:" << target_tp.get_name();
    }

}







std::vector<std::string> foxtrot::Device::GetCapabilityNames() const
{
    std::vector<std::string> out;
    auto tp = rttr::type::get(*this);
    auto props = tp.get_properties();
    auto meths = tp.get_methods();
    
    out.reserve(props.size() + meths.size());
    
        for(auto& prop : props)
    {
     out.push_back(std::string{prop.get_name()});
    }
    
    for(auto& meth: meths)
    {
        out.push_back(std::string{meth.get_name()});
    };
    
    return out;
    
}

rttr::variant foxtrot::Device::Invoke(const Capability& cap, foxtrot::rarg_cit argbegin,
                                      foxtrot::rarg_cit argend)
{
    if(cap.invokable.has_value())
        return Invoke(*(cap.invokable), argbegin, argend);
    else
        return Invoke(cap.CapabilityName, argbegin, argend);

}


rttr::variant foxtrot::Device::Invoke(const std::variant< rttr::property, rttr::method >& invokable, foxtrot::rarg_cit beginargs, foxtrot::rarg_cit endargs)
{
        rttr::variant ret = std::visit([&beginargs, &endargs, this] (auto&& v)
        {

	  rttr::variant out;
            using T = std::decay_t<decltype(v)>;

            if constexpr(std::is_same_v<T, rttr::property>)
	      {
                auto nargs = std::distance(beginargs,endargs);
                if(v.is_readonly())
		  {
		    out = v.get_value(*this);
		    return out;
		  }
                else if(nargs > 1)
		  {
		    throw std::logic_error("require only 1 argument to writable property, this may be an error in your device driver");
		  }
                else if(nargs == 1)
		  {
		    bool success;
		    auto argcpy = *beginargs;
		    sanitize_arg(argcpy,v.get_type(),1);
		    success = v.set_value(*this, argcpy);
		    if(!success)
		      throw std::runtime_error("failed to set property!");

		    //NOTE: return void here because that's what the other side expects!
		    auto voidmeth = rttr::type::get_global_method("void_helper_function");
		    out = voidmeth.invoke({});

		    return out;;
		  }
                else if(nargs == 0)
		  {
		    out = v.get_value(*this);
		    return out;
		  }
	      }
            else if constexpr(std::is_same_v<T, rttr::method>)
	      {
                //method call
                if(!v.is_valid())
                    throw std::logic_error("invalid method!");
		
                std::vector<rttr::variant> argcopy(beginargs, endargs);
        //TODO: check arguments here
                auto paraminfs = v.get_parameter_infos();
                auto paraminfsit = paraminfs.begin();
                if(argcopy.size() != paraminfs.size())
                    throw std::runtime_error("wrong number of arguments provided");

                int i =1;
                for(auto& a : argcopy)
		  {
                    auto target_tp = (paraminfsit++)->get_type();
                    if(a.get_type().is_wrapper())
		      {
                        lg_.strm(sl::warning) << "an argument type ended up as a wrapper. This will cause a copy and is inefficient";
                        lg_.strm(sl::warning) << "the argument type is:"  << a.get_type().get_name().to_string();
                        a = a.extract_wrapped_value();
		      }

                    sanitize_arg(a, target_tp, i++, &lg_);
                }

                std::vector<rttr::argument> argvec(argcopy.begin(), argcopy.end());
                out = v.invoke_variadic(*this,argvec);
                if(!out)
                    throw std::logic_error("failed to invoke method!");
                return out;
            }
            else
            {
                static_assert("non exhaustive visit!");
            }

	    return out;

        }, invokable);

        return ret;
}


rttr::variant foxtrot::Device::Invoke(const std::string& capname, foxtrot::rarg_cit argbegin,
                                      foxtrot::rarg_cit argend)
{
    lg_.strm(sl::trace) << "cap string registry size is: "<< _cap_string_registry.size();
    auto capid = _cap_string_registry.at(capname);
    lg_.strm(sl::debug) << "found capid: " << capid;
    return Invoke(capid, argbegin, argend);
}

rttr::variant foxtrot::Device::Invoke(unsigned short capid, foxtrot::rarg_cit beginargs,
                                      foxtrot::rarg_cit endargs)
{

    auto pm = _cap_registry.at(capid).invokable;
    lg_.strm(sl::trace) << "found invokable in registry";
    if(!pm.has_value())
        throw std::logic_error("invokable in table has no value! This should NEVER happen");

    return Invoke(*pm, beginargs, endargs);
}

foxtrot::Capability foxtrot::Device::GetCapability(const std::string& capname) const
{
    auto reflecttp = rttr::type::get(*this);

    auto prop = reflecttp.get_property(capname.c_str());
    auto meth = reflecttp.get_method(capname.c_str());
    
    Capability out;
    out.CapabilityName = capname;    

    rttr::variant flagmeta;
    rttr::variant flagmaskmeta;
    
    if(prop)
        return GetCapability(prop);
    else if(meth)
        return GetCapability(meth);
    else
        throw std::out_of_range("requested capability doesn't seem to exist!");

    bool both_metas = flagmeta.is_valid() and flagmaskmeta.is_valid();
    
    return out;
    
}


std::optional<foxtrot::Lock> foxtrot::Device::obtain_lock(const foxtrot::Capability& cap)
{
    lg_.strm(sl::trace) << "lock_device default implementation called";
    std::optional<Lock> out;
    return out;
}


bool foxtrot::Device::hasLockImplementation() const {return false;};

void void_helper_function()
{
};


bool foxtrot::Device::Reconnect()
{
    return false;
};

bool foxtrot::Device::Reset()
{
    return false;
}

void foxtrot::Device::load_capability_map(bool force_reload)
{

  lg_.strm(sl::debug) << "loading capability map...";
  
    if(!_registry_is_loaded or force_reload)
    {
      lg_.strm(sl::trace) << "in capability map load loop";
      
        _cap_registry.clear();
        _cap_string_registry.clear();
        unsigned short idx = 0;
        auto reflecttp = rttr::type::get(*this);

	lg_.strm(sl::debug) << "reflecttp name is: " << reflecttp.get_name();
	
        for(auto& prop : reflecttp.get_properties())
        {
	  lg_.strm(sl::trace) << "prop: " << prop.get_name();
	   auto cap = GetCapability(prop);
            _cap_registry.insert({idx, cap});
            _cap_string_registry.insert({cap.CapabilityName, idx++});
        }
        for(auto& meth : reflecttp.get_methods())
        {
	  lg_.strm(sl::trace) << "meth: " << meth.get_name();
	  auto cap = GetCapability(meth);
            _cap_registry.insert({idx, cap});
            _cap_string_registry.insert({cap.CapabilityName, idx++});
        }
    }
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::Device;
 registration::class_<Device>("foxtrot::Device")
 .property_readonly("getDeviceTypeName", &Device::getDeviceTypeName)
 .property_readonly("getDeviceComment", &Device::getDeviceComment)
 .method("Reconnect", &Device::Reconnect)
 .method("Reset", &Device::Reset)
 ;
 
 registration::method("void_helper_function", &void_helper_function);
       
}





