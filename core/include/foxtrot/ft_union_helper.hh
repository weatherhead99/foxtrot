#pragma once
#include <rttr/registration>
#include <variant>
#include <foxtrot/ft_types.pb.h>

namespace foxtrot
{
  namespace detail
  {
 
    template<typename T>
    rttr::variant union_get(T& in)
    {
      auto outer_var = std::visit([] (auto& arg) {
	return rttr::variant(arg);
	}, in);
      return outer_var;
    }

  template<typename T>
    rttr::type union_held_type(const T& in)
    {

      rttr::type out = rttr::type::get<void>();
     
      std::visit([&in, &out] (auto& arg) {

	using A = typename std::decay<decltype(arg)>::type;
	if(std::holds_alternative<A>(in))
	  {
	    rttr::variant var = arg;
	    out = var.get_type();
	  }

      }, in);

      return out;
      
    }

  template<typename T>
  std::vector<rttr::type> union_possible_types(const T& in)
  {

    auto void_ = rttr::type::get<void>();
    auto sz = std::variant_size<T>::value;
    std::vector<rttr::type> out(sz, void_);

    auto it = out.begin();

    std::visit([&it] (auto& arg)
    {
      using A = typename std::decay<decltype(arg)>::type;
      *(it++) = rttr::type::get<A>();

    }, in);

    return out;

  }


    template<typename T>
    struct converter_helper {};
       

  }



  template<typename T>
  void register_union()
  {
    
    auto tp = rttr::type::get<T>();
    static auto nm = tp.get_name().to_string();
    rttr::registration::class_<T> reg(nm);

    reg(rttr::metadata("unionmeta", true),
	rttr::metadata("ft_type", variant_types::UNION_TYPE))
      .method("get", &detail::union_get<T>)
      .method("held_type", &detail::union_held_type<T>);
      //      .method("possible_types", &detail::union_possible_types<T>);

  }


  rttr::variant union_get(const rttr::variant& var);
  

}
