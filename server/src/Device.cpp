#include <sstream>

#include <rttr/registration>

#include <foxtrot/server/Device.h>
#include <foxtrot/ReflectionError.h>


foxtrot::Device::Device(std::shared_ptr< foxtrot::CommunicationProtocol > proto, const std::string& comment)
: _proto(std::move(proto)), _devcomment(comment), lg_("Device")
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


class ftarg_visitor : public boost::static_visitor<>
{
public:
    ftarg_visitor(rttr::variant& var) : var_(var) {};
    void operator()(double& i) const
    {
        var_ = i;
    }
    void operator()(int& i) const
    {
        var_ = i;
    }
    void operator()(bool& i) const
    {
        var_ = i;
    }
    void operator()(const std::string& s) const
    {
        var_ = s;
    }
    
private:
    rttr::variant& var_;
};


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
            if(a.get_type().is_wrapper())
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
            
            auto voidtp = rttr::type::get<void>();
            auto voidval = voidtp.create();
            return voidval;
        }
        else if(nargs == 0)
        {
            auto retval = prop.get_value(*this);
            return retval;
        }
    }
    
    
    
}

foxtrot::Capability foxtrot::Device::GetCapability(const std::string& capname) const
{
    auto reflecttp = rttr::type::get(*this);
    auto prop = reflecttp.get_property(capname.c_str());
    auto meth = reflecttp.get_method(capname.c_str());
    
    Capability out;
    out.CapabilityName = capname;
    
    if(prop)
    {
        auto proptp = prop.get_type();
        out.Returntype = proptp;
        
        
        if(proptp.is_sequential_container())
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
    }
    else if(meth)
    {
        if(meth.get_return_type().is_sequential_container())
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
    }
    else
    {
        throw std::out_of_range("requested capability which doesn't seem to exist!");
    }
    
    return out;
    
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::Device;
 registration::class_<Device>("foxtrot::Device")
 .property_readonly("getDeviceTypeName", &Device::getDeviceTypeName)
 .property_readonly("getDeviceComment", &Device::getDeviceComment)
 ;
       
}





