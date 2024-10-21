#include <memory>

#include <rttr/type>
#include <rttr/variant.h>

#include <foxtrot/Logging.h>

#include <foxtrot/server/ServerUtil.h>
#include <foxtrot/typeUtil.h>
#include <proto/foxtrot.pb.h>
#include <foxtrot/ReflectionError.h>

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
        if(lg)
            lg->strm(sl::trace) << "in is valid? " << in.is_valid() ;
        *retval = get_variant_wire_type(in, lg);
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



std::unique_ptr<unsigned char[]>  foxtrot::byte_view_data(rttr::variant& arr, unsigned int& byte_size, foxtrot::byte_data_types& dt)
{
    foxtrot::Logging lg("byte_view_data");
    
    if(!arr.get_type().is_sequential_container())
    {
      lg.Error("asked for byte view of a type that is not a sequential container!");
      throw std::logic_error("type not a sequential container!");
    }
    
    
    lg.strm(sl::debug) << "variant type: " << arr.get_type().get_name();

    auto view = arr.create_sequential_view();
    if(view.get_rank() > 1)
      {
	lg.Error("can only deal with arrays of rank 1!");
	throw foxtrot::ReflectionError("unable to deal with arrays of rank > 1");
      }

   
    
    dt = foxtrot::get_byte_data_type(view.get_rank_type(1), &lg);
    auto dat = foxtrot::variant_to_bytes(arr, &lg);
    byte_size = dat.size();
    
    auto out = std::unique_ptr<unsigned char[]>(new unsigned char[dat.size()]);

    
    std::copy(dat.begin(), dat.end(), out.get());
    
    return out;

    

    
}


