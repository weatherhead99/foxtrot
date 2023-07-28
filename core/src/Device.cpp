#include <sstream>
#include <algorithm>
#include <variant>

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


foxtrot::Device::Device(std::shared_ptr< foxtrot::CommunicationProtocol > proto, const std::string& comment)
: _proto(std::move(proto)), _devcomment(comment), lg_("Device")
{
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




template<typename T> bool is_ft_call_streaming ( const T& propmeth )
{
    auto streammeta = propmeth.get_metadata ( "streamdata" );
    if ( streammeta.is_valid() ) {
        if ( streammeta.to_bool() ) {
            return true;
        };

    };
    return false;
};



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
    return Invoke(cap.CapabilityName, argbegin, argend);
}


rttr::variant foxtrot::Device::Invoke(const std::string& capname, foxtrot::rarg_cit argbegin,
                                      foxtrot::rarg_cit argend)
{
       //default (rttr based) implementation. Override in e.g. python devices
    auto devtp = rttr::type::get(*this);
    
    auto prop = devtp.get_property(capname.c_str());
    auto meth = devtp.get_method(capname.c_str());
    
    if(!meth && !prop)
    {
        throw ReflectionError("no matching property or method");
    }
    else if(!prop)
    {
        //method call
        if(!meth.is_valid())
        {
            throw std::logic_error("invalid method!");
        }
        
        std::vector<rttr::variant> argcopy(argbegin, argend);
        //TODO: check arguments here
        auto paraminfs = meth.get_parameter_infos();
        auto paraminfsit = paraminfs.begin();
        if(argcopy.size() != paraminfs.size())
        {
            throw std::runtime_error("wrong number of arguments provided");
        }
        
        int i =1;
        for(auto& a : argcopy)
        {
            auto target_tp = (paraminfsit++)->get_type();
	    
	    
            if(a.get_type().is_wrapper() and ! target_tp.is_wrapper())
            {
                lg_.strm(sl::warning) << "in capability with name: " << capname;
                lg_.strm(sl::warning) << "an argument type ended up as a wrapper. This will cause a copy and is inefficient";
                lg_.strm(sl::warning) << "the argument type is:"  << a.get_type().get_name().to_string();
                
                a = a.extract_wrapped_value();
            }
            
            
            sanitize_arg(a, target_tp, i++, &lg_);
        }
        
        std::vector<rttr::argument> argvec(argcopy.begin(), argcopy.end());
        auto retval = meth.invoke_variadic(*this,argvec);
        if(!retval)
            throw std::logic_error("failed to invoke method!");
        return retval;
        
    }
    else
    {
        auto nargs = std::distance(argbegin,argend);
        if(prop.is_readonly())
        {
            auto ret = prop.get_value(*this);
            return ret;
        }
        else if(nargs > 1)
        {
            throw std::logic_error("require only 1 argument to writable property, this may be an error in your device driver");
        }
        else if(nargs == 1)
        {
            bool success;
            auto argcpy = *argbegin;
            sanitize_arg(argcpy,prop.get_type(),1);
            success = prop.set_value(*this, argcpy);
            if(!success)
                throw std::runtime_error("failed to set property!");
            
//             auto voidtp = rttr::type::get<void>();
//             auto voidval = voidtp.create();
//HACK: creating void type doesn't seem to work somehow!
            auto voidmeth = rttr::type::get_global_method("void_helper_function");
            auto voidret = voidmeth.invoke({});
            
            return voidret;
        }
        else if(nargs == 0)
        {
            auto retval = prop.get_value(*this);
            return retval;
        }
        
        throw std::logic_error("code should never reach this point, there is a programming error");
    }
    
    
    
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
    {
        auto proptp = prop.get_type();
        out.Returntype = proptp;
        
        
        if(is_ft_call_streaming(prop))
        {
            out.type = CapabilityType::STREAM;
        }
        
        else if(prop.is_readonly())
        {
            out.type = CapabilityType::VALUE_READONLY;
        }
        else
        {
            out.type = CapabilityType::VALUE_READWRITE;
            out.Argnames.push_back(std::string{proptp.get_name()});
            out.Argtypes.push_back(proptp);
        }

	flagmeta = prop.get_metadata("ft_flags");
	flagmaskmeta = prop.get_metadata("ft_flagmask");
       
    }
    else if(meth)
    {
        
        if(is_ft_call_streaming(meth))
        {
            out.type = CapabilityType::STREAM;
        }
        else
        {
            out.type = CapabilityType::ACTION;
        }
        
        auto args = meth.get_parameter_infos();
        auto rettp = meth.get_return_type();
        
        out.Returntype = rettp;
        out.Argnames.reserve(args.size());
        out.Argtypes.reserve(args.size());
        
        for(auto& arg : args)
        {
            out.Argnames.push_back(std::string(arg.get_name()));
            out.Argtypes.push_back(arg.get_type());
        }

	flagmeta = meth.get_metadata("ft_flags");
	flagmaskmeta = meth.get_metadata("ft_flagmask");

    }
    else
    {
        throw std::out_of_range("requested capability which doesn't seem to exist!");
    }

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


RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::Device;
 registration::class_<Device>("foxtrot::Device")
 .property_readonly("getDeviceTypeName", &Device::getDeviceTypeName)
 .property_readonly("getDeviceComment", &Device::getDeviceComment)
 ;
 
 registration::method("void_helper_function", &void_helper_function);
       
}





