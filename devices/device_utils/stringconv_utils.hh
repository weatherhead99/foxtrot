#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>
#include <charconv>
#include <foxtrot/Logging.h>

namespace foxtrot
{
  class Logging;
  
  namespace devices
  {

    template<typename Ret, int Base=10>
    Ret number_from_string(const std::string& in,
		       foxtrot::Logging* lg = nullptr)
    {
      Ret out;
      std::from_chars_result res{};

      if constexpr(std::is_same_v<Ret, bool>)
	{
	  if(lg)
	    lg->strm(sl::trace) << "converting bool type";
	  return (bool) std::stoi(in);
	}
	else if constexpr(std::is_integral_v<Ret>)
	{
	  if(lg)
	    lg->strm(sl::trace) << "converting integral type";
	  res = std::from_chars(in.data(),
				in.data() + in.size(), out, Base);
	}
      else if constexpr(std::is_floating_point_v<Ret>)
	{
	  if(lg)
	    lg->strm(sl::trace) << "converting floating point type";
	  if constexpr(Base == 16)
	    res = std::from_chars(in.data(),
				  in.data() + in.size(), out,
				  std::chars_format::hex);
	  else
	    res = std::from_chars(in.data(),
				  in.data() + in.size(), out,
				  std::chars_format::general);
	}
      else
	{
	[]<bool flag=false>() {
	  static_assert(flag, "unknown type to number_from_string");
	};

	return out;

	}

      if(res.ptr != nullptr)
	if(lg)
	  lg->strm(sl::debug) << "remaining text was:{ " << *res.ptr << "}";

      if(res.ec != std::errc())
	{
	  if(lg)
	    lg->strm(sl::error) << "errc: " << static_cast<int>(res.ec);
	throw std::runtime_error("got error code converting number");
	}

      return out;
    }
      

  }
    
}
