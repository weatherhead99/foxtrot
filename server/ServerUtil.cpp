#include "ServerUtil.h"
#include "Logging.h"
#include <rttr/type>
#include <rttr/variant.h>

#include <memory>


rttr::variant foxtrot::get_arg(const capability_argument& arg, bool& success)
    {
    success=true;
    rttr::variant outarg;
    auto which_type = arg.value_case();
    
    foxtrot::Logging lg("get_arg");
    lg.Debug("arg type switch: " + std::to_string(which_type) );
    
    switch(which_type)
    {
    case(capability_argument::ValueCase::kDblarg):
        outarg = arg.dblarg();
        break;
        
    case(capability_argument::ValueCase::kIntarg):
        outarg = arg.intarg();
        break;
        
    case(capability_argument::ValueCase::kBoolarg):    
        outarg = arg.boolarg();
        break;
    
    case(capability_argument::ValueCase::kStrarg):
        outarg = arg.strarg();
        break;

    case(capability_argument::ValueCase::VALUE_NOT_SET):
        success = false;
    }
  
    //     cout << "outarg type: " << outarg.get_type().get_name() << endl;
    return outarg;
    }
    

    
bool foxtrot::set_returntype(rttr::variant& retval, capability_response& repl)
{
      foxtrot::Logging _lg("set_returntype");
  
      _lg.Trace("setting return type" );
      _lg.Trace("raw type name is: " + retval.get_type().get_name());
      auto rettp = get_appropriate_wire_type(retval);
        _lg.Trace("rettp is: " + std::to_string(rettp) );
            bool convertsuccess = true;
         if(rettp == value_types::FLOAT)
         {
	   _lg.Trace("it's a double!");
            repl.set_dblret(retval.to_double(&convertsuccess));
         }
         else if (rettp == value_types::BOOL)
         {
	   _lg.Trace("bool");
             convertsuccess = retval.can_convert(rttr::type::get<bool>());
             repl.set_boolret(retval.to_bool());
         }
         else if(rettp == value_types::INT)
         {
             _lg.Trace("int");
             repl.set_intret(retval.to_int(&convertsuccess));
         }
         else if(rettp == value_types::STRING)
         {
             _lg.Trace("string");
	     
	     auto canconv = retval.can_convert(rttr::type::get<std::string>());
	     _lg.Trace("can convert to string: " + std::to_string(canconv));
	     
	     auto ok = retval.convert(rttr::type::get<std::string>());
	     
	     _lg.Trace("converted string: " + retval.to_string());
	     
	     bool ok2;
	     std::string out = retval.convert<std::string>(&ok2);
	     _lg.Trace("retval.convert: " + out);
	     
             repl.set_stringret(retval.to_string(&convertsuccess));
         }
         //if it's VOID, no need to set rettp
         else if(rettp == value_types::VOID)
	 {
	    _lg.Trace("void");
	    repl.set_stringret("");
	   
	 }
         
         if(!convertsuccess)
         {
             errstatus* errstat = repl.mutable_err();
             errstat->set_msg("couldn't successfully convert return type");
             errstat->set_tp(error_types::Error);
         };
         
         _lg.Debug("convertsuccess: " + std::to_string(convertsuccess) );
         return convertsuccess;
}

foxtrot::value_types foxtrot::get_appropriate_wire_type(const rttr::variant& vr)
{
    using namespace rttr;
    
    auto tp = vr.get_type();
    
    if(tp == type::get<void>())
    {
        return value_types::VOID;
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
    
    if( tp == type::get<std::string>())
    {
      return value_types::STRING;
    }
    
    if(vr.can_convert<int>() || vr.can_convert<unsigned>() )
    {
      return value_types::INT;
    }
    
    if(!tp.is_arithmetic() )
    {
        return value_types::STRING;
    }
    
    
    return value_types::INT;
}



template <typename T>  std::unique_ptr<unsigned char[]> variant_to_bytes(rttr::variant& vt, unsigned& byte_size)
{ 
    
    rttr::type tp = rttr::type::get<std::vector<T>>();
    if(!vt.can_convert(tp))
    {
        return nullptr;
    }
    
    auto arr = vt.convert<std::vector<T>>();
    
    byte_size = sizeof(T) / sizeof(unsigned char) * arr.size();
    auto data  = std::unique_ptr<unsigned char[]>(new unsigned char[byte_size]);
    
    auto targetptr = reinterpret_cast<unsigned char*>(arr.data());
    
    std::copy(targetptr , targetptr + byte_size, data.get());
    
    return data;
    
}


std::unique_ptr<unsigned char[]>  foxtrot::byte_view_data(rttr::variant& arr, unsigned int& byte_size, foxtrot::byte_data_types& dt)
{
    foxtrot::Logging lg("byte_view_data");
    std::unique_ptr<unsigned char[]> data;
    //find type
    if(data = variant_to_bytes<unsigned char>(arr,byte_size))
    {
      lg.Trace("UCHAR");
     dt = foxtrot::byte_data_types::UCHAR;   
     return data;
    }
    else if(data = variant_to_bytes<unsigned short>(arr,byte_size))
    {
      lg.Trace("USHORT");
        dt = foxtrot::byte_data_types::USHORT;
        return data;
    }
    else if(data = variant_to_bytes<unsigned int>(arr,byte_size))
    {
      lg.Trace("UINT");
        dt = foxtrot::byte_data_types::UINT;
        return data;
    }
    else if(data = variant_to_bytes<unsigned long>(arr,byte_size))
    {
      lg.Trace("ULONG");
        dt = foxtrot::byte_data_types::ULONG;
        return data;
    }
    else if(data = variant_to_bytes<short>(arr,byte_size))
    {
      lg.Trace("SHORT");
        dt = foxtrot::byte_data_types::SHORT;
        return data;
    }
    else if(data= variant_to_bytes<int>(arr,byte_size))
    {
      lg.Trace("IINT");
        dt = foxtrot::byte_data_types::IINT;
        return data;
    }
    else if(data =  variant_to_bytes<long>(arr,byte_size))
    {
      lg.Trace("LONG");
        dt = foxtrot::byte_data_types::LONG;
        return data;
    }
    else if(data = variant_to_bytes<float>(arr,byte_size))
    {
      lg.Trace("BFLOAT");
        dt = foxtrot::byte_data_types::BFLOAT;
        return data;
    }
    else if(data = variant_to_bytes<double>(arr,byte_size))
    {
      lg.Trace("BDOUBLE");
        dt = foxtrot::byte_data_types::BDOUBLE;
        return data;
    }
    
    throw std::logic_error("function couldn't convert to recognized array type...");
    
}


