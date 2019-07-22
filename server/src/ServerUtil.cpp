#include <memory>

#include <rttr/type>
#include <rttr/variant.h>

#include <foxtrot/Logging.h>

#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/typeUtil.h>
#include <foxtrot/foxtrot.pb.h>

using namespace foxtrot;

rttr::variant foxtrot::wire_arg_to_variant(const foxtrot::capability_argument& arg, bool& success, const rttr::type& target_tp, foxtrot::Logging* lg)
{
    success = true;
    rttr::variant out;
    
    try
    {
        out = wire_type_to_variant(arg.value(), target_tp);
    }
    catch(std::runtime_error& err)
    {
        if(lg)
        {
            lg->strm(sl::error) << "caught runtime error in converting argument";
            lg->strm(sl::error) << "details: " << err.what();
        }
        throw err;
    };
    return out;
}

void foxtrot::set_retval_from_variant(const rttr::variant& in, foxtrot::capability_response& resp, foxtrot::Logging* lg)
{
    ft_variant* retval = resp.mutable_returnval();
    try{
        *retval = get_variant_wire_type(in);
    }
    catch(std::logic_error& err)
    {
        if(lg)
        {
            lg->strm(sl::error) << "caught logic error converting variant wire type";
            lg->strm(sl::error) << "details: " << err.what();
        }
        throw err;
    }

}


// foxtrot::value_types foxtrot::get_appropriate_wire_type(const rttr::variant& var)
// {
//   
//   if(!var.is_valid())
//   {
//     throw std::logic_error("variant supplied to get_appropriate_wire_type is invalid!");
//   }
//   
//   auto tp = var.get_type();
//   
//   if(!tp.is_valid())
//   {
//     throw std::logic_error("invalid type check previous!");
//   }
//   
//   
// //   if( var.can_convert<int>() || var.can_convert<unsigned>() )
// //     {
// //       if(tp != rttr::type::get<double>() && tp != rttr::type::get<float>())
// // 	{
// // 	  return value_types::INT;
// // 	}
// //     }
// //     
//   return get_appropriate_wire_type(tp);
// }
// 
// foxtrot::value_types foxtrot::get_appropriate_wire_type(const rttr::type& tp)
// {
//     using namespace rttr;
//     
//     
//     if(!tp.is_valid())
//     {
//         throw std::logic_error("invalid type supplied to get_appropriate_wire_type");
//     }
//     
//     
//     if(tp == type::get<void>())
//     {
//         return value_types::VOID_TYPE;
//     }
//     
//     //check for bool
//     if(tp == type::get<bool>())
//     {
//         return value_types::BOOL_TYPE;
//     }
//     
//     //check for float
//     if( (tp == type::get<double>()) || (tp == type::get<float>())) 
//     {
//         return value_types::FLOAT_TYPE;
//     }
//     
//     if( tp == type::get<std::string>())
//     {
//       return value_types::STRING_TYPE;
//     }
//     
//     if(tp.is_enumeration())
//     {
//       return value_types::INT_TYPE;
//     }
//     
//     
//     if(!tp.is_arithmetic() )
//     {
//         return value_types::STRING_TYPE;
//     }
//     
//     
//     return value_types::INT_TYPE;
// }



template <typename T>  std::unique_ptr<unsigned char[]> variant_to_bytes(rttr::variant& vt, unsigned& byte_size)
{ 
    foxtrot::Logging lg("variant_to_bytes");
#ifndef NEW_RTTR_API
    rttr::type tp = rttr::type::get<std::vector<T>>();
    lg.strm(sl::debug) << "reference type: " << tp.get_name();
    lg.strm(sl::debug) << "received type: " << vt.get_type().get_name();
    
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

#else
  lg.strm(sl::trace) << "received type: " << vt.get_type().get_name();
  
  auto view = vt.create_sequential_view();
  auto value_type = view.get_value_type();

  lg.strm(sl::trace) << " value type: " << value_type.get_name();
  
  if(rttr::type::get<T>() != value_type)
  {
      lg.Trace("unequal types...");
      return nullptr;
  }
  
  byte_size = sizeof(T) / sizeof(unsigned char) * view.get_size();
  auto data = std::unique_ptr<unsigned char[]>(new unsigned char[byte_size]);
  
  auto& source = rttr::variant_cast<std::vector<T>&>(vt);
  auto targetptr = reinterpret_cast<unsigned char*>(source.data());
  std::copy(targetptr, targetptr + byte_size, data.get());
  
  return data;
  
#endif
    
}

std::unique_ptr<unsigned char[]> old_rttr_array_converter(rttr::variant& arr, foxtrot::Logging& lg, unsigned int& byte_size, foxtrot::byte_data_types& dt)
{
  std::unique_ptr<unsigned char[]> data;
    //find type
    if(data = variant_to_bytes<unsigned char>(arr,byte_size))
    {
      lg.Trace("UCHAR");
     dt = foxtrot::byte_data_types::UCHAR_TYPE;   
     return data;
    }
    else if(data = variant_to_bytes<unsigned short>(arr,byte_size))
    {
      lg.Trace("USHORT");
        dt = foxtrot::byte_data_types::USHORT_TYPE;
        return data;
    }
    else if(data = variant_to_bytes<unsigned int>(arr,byte_size))
    {
      lg.Trace("UINT");
        dt = foxtrot::byte_data_types::UINT_TYPE;
        return data;
    }
    else if(data = variant_to_bytes<unsigned long>(arr,byte_size))
    {
      lg.Trace("ULONG");
        dt = foxtrot::byte_data_types::ULONG_TYPE;
        return data;
    }
    else if(data = variant_to_bytes<short>(arr,byte_size))
    {
      lg.Trace("SHORT");
        dt = foxtrot::byte_data_types::SHORT_TYPE;
        return data;
    }
    else if(data= variant_to_bytes<int>(arr,byte_size))
    {
      lg.Trace("IINT");
        dt = foxtrot::byte_data_types::IINT_TYPE;
        return data;
    }
    else if(data =  variant_to_bytes<long>(arr,byte_size))
    {
      lg.Trace("LONG");
        dt = foxtrot::byte_data_types::LONG_TYPE;
        return data;
    }
    else if(data = variant_to_bytes<float>(arr,byte_size))
    {
      lg.Trace("BFLOAT");
        dt = foxtrot::byte_data_types::BFLOAT_TYPE;
        return data;
    }
    else if(data = variant_to_bytes<double>(arr,byte_size))
    {
      lg.Trace("BDOUBLE");
        dt = foxtrot::byte_data_types::BDOUBLE_TYPE;
        return data;
    }
    throw std::logic_error("function couldn't convert to recognized array type...");
    
};


std::unique_ptr<unsigned char[]>  foxtrot::byte_view_data(rttr::variant& arr, unsigned int& byte_size, foxtrot::byte_data_types& dt)
{
    foxtrot::Logging lg("byte_view_data");
    
    if(!arr.get_type().is_sequential_container())
    {
      lg.Error("asked for byte view of a type that is not a sequential container!");
      throw std::logic_error("type not a sequential container!");
    }
    
    
    lg.strm(sl::debug) << "variant type: " << arr.get_type().get_name();
    

    return old_rttr_array_converter(arr,lg,byte_size,dt);
    
}


