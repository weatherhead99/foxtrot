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

foxtrot::ft_returntype get_returnval(rttr::variant& var, foxtrot::Logging* lg = nullptr)
{
    foxtrot::ft_returntype out;
    
    if(!var.is_valid())
        throw std::logic_error("invalid RTTR variant supplied to get_returnval");
    
    auto tp = var.get_type();
    if(!tp.is_valid())
        throw std::logic_error("invalid RTTR type from variant in get_returnval");
    
    bool success = false;
    
    if(tp == rttr::type::get<void>())
    {
        //don't set optional
        return out;
    }

    foxtrot::ft_returntype retval;
    if(tp == rttr::type::get<bool>())
    {
        retval = var.to_bool();
        success = true;
    }
    
    if(tp == rttr::type::get<double>() || tp == rttr::type::get<float>())
    {
        if(lg)
            lg->strm(sl::trace) << "return type is a double";
        retval = var.to_double(&success);
        if(lg)
            lg->strm(sl::trace) << "retval value stored: " << boost::get<double>(*retval);
    }
    
    if(tp == rttr::type::get<std::string>())
    {
        retval = std::move(var.to_string(&success));
    }
    
    if(tp.is_enumeration())
    {
        retval = var.to_int(&success);
    }

    if(!tp.is_arithmetic())
    {
        retval = std::move(var.to_string(&success));
    }

    if(!success)
    {
        if(lg)
            lg->strm(sl::trace) << "return type fallthrough, using int";
        retval = var.to_int(&success);
    }
    
    if(!success)
    {
        throw std::logic_error("failed to convert return type. This may be an error in your device driver");
    }
    
    out = std::move(retval);
    return out;

};


rttr::variant get_arg(foxtrot::ft_argtype& argin, const rttr::type& tp, int pos, foxtrot::Logging* lg =nullptr)
{
    rttr::variant out;
    boost::apply_visitor(ftarg_visitor(out),argin);
    if(!out.is_valid())
    {
        throw std::logic_error("variant is invalid! in get_arg");
    }
    if(lg)
        lg->strm(sl::trace) << "target argtype: " << tp.get_name();
    
    if(!out.can_convert(tp))
    {
        std::ostringstream oss;
        oss << "argument at position: " << pos << "of type: " << out.get_type().get_name() << "cannot be converted to needed type: " << tp.get_name();
        throw std::logic_error(oss.str());
    }
        
    if(out.get_type() != tp)
    {
        if(lg)
            lg->strm(sl::trace) << "need to convert";
        bool success = out.convert(tp);
        if(!success)
            throw foxtrot::ReflectionError("failed to convert argument at position: " + std::to_string(pos));
    };
    
    if(lg)
        lg->strm(sl::trace) << "printing prepared arg: " << out.to_string();
    
    if(lg)
    {
        lg->strm(sl::trace) << "argument type: " << out.get_type().get_name();
        lg->strm(sl::trace) << "target type:" << tp.get_name();
    }
        
    
    return out;
}

std::vector<rttr::variant> get_callargs(rttr::method& meth,
                                        foxtrot::arg_cit begin, foxtrot::arg_cit end, foxtrot::Logging* lg = nullptr)
{
    auto argsize_given = std::distance(begin,end);
    auto param_infs = meth.get_parameter_infos();
    
    if(argsize_given != param_infs.size())
    {
        throw std::out_of_range("unexpected number of arguments supplied");
    }
    
    std::vector<rttr::variant> out;
    out.reserve(std::distance(begin,end));
    
    auto paraminfsit = param_infs.begin();
    int i=0;
    for(auto it = begin; it != end; it++)
    {
        const auto target_argtp = (paraminfsit++)->get_type();
        i++;
        //HACK: this could be much better and avoid copying, probably
        auto argcpy = *it;
        auto rttrarg = get_arg(argcpy,target_argtp,i+1,lg);
        out.push_back(rttrarg);
    };
    
    return out;
};


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


foxtrot::ft_returntype foxtrot::Device::Invoke(const std::string& capname,
    foxtrot::arg_cit beginargs, foxtrot::arg_cit endargs)
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
        if(is_ft_call_streaming(meth))
        {
            throw foxtrot::ReflectionError("tried to InvokeCapability on a bulk data method!");
        }
        
        auto callargs = get_callargs(meth, beginargs, endargs, &lg_);
        std::vector<rttr::argument> argvec(callargs.begin(), callargs.end());
        auto retval = meth.invoke_variadic(*this,argvec);
        if(!retval)
            throw std::logic_error("failed to invoke method!");
        return get_returnval(retval, &lg_);
        
    }
    else
    {
        //property
        if(is_ft_call_streaming(prop))
        {
            throw foxtrot::ReflectionError("tried to InvokeCapability on a bulk data method!");
        }
        auto nargs = std::distance(beginargs,endargs);
        
        if(prop.is_readonly())
        {
            auto ret = prop.get_value(*this);
            return get_returnval(ret, &lg_);
        }
        else if(nargs > 1)
        {
            throw std::logic_error("require only 1 argument to writable property, this may be an error in your device driver");
        }
        else if(nargs == 1)
        {
            bool success;
            auto argcpy = *beginargs;
            auto arg = get_arg(argcpy,prop.get_type(),1);
            success = prop.set_value(*this, arg);
            if(!success)
                throw std::runtime_error("failed to set property!");
            foxtrot::ft_returntype ret;
            return ret;
        }
        else if(nargs == 0)
        {
            auto retval = prop.get_value(*this);
            return get_returnval(retval, &lg_);
        }
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





