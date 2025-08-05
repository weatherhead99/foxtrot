#pragma once
#include <rttr/registration>
#include <rttr/type>
#include <variant>
#include <ft_types.pb.h>

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

    
    template<typename Variant, std::size_t N = std::variant_size_v<Variant> -1>
    struct variant_possible_types_helper
    {
      using Tp = std::variant_alternative_t<N,Variant>;
      constexpr static void build_type_vector(std::vector<rttr::type>& in)
      {
	rttr::type tpname = rttr::type::get<Tp>();
	in.push_back( tpname);
	if constexpr(N != 0)
	  variant_possible_types_helper<Variant, N-1>::build_type_vector(in);
      }
    };

    template<typename Variant>
    std::vector<rttr::type> union_possible_types(const Variant& in)
    {
      std::vector<rttr::type> out;
      out.reserve(std::variant_size_v<Variant>);

      namespace detail = foxtrot::detail;

      detail::variant_possible_types_helper<Variant>::build_type_vector(out);
      return out;
      
    };
    
    template<typename Variant, std::size_t N = std::variant_size_v<Variant> -1>
    struct variant_converter_reg
    {
      using From = std::variant_alternative_t<N,Variant>;
      
      constexpr static void reg()
      {
	auto conv_in = [](From in, bool& ok) -> Variant
	{
	  ok = true;
	  return Variant{in};
	};
	rttr::type::register_converter_func(conv_in);

        auto conv_out = [](Variant in, bool& ok) -> From
	{
	  if(std::holds_alternative<From>(in))
	    {
	      ok = true;
	      return std::get<From>(in);
	    }
	  ok = false;
	  From out;
	  return out;
	};

	rttr::type::register_converter_func(conv_out);

	if constexpr(N == 0)
	  return;
	else
	  return variant_converter_reg<Variant, N-1>::reg();
	
      }

    };

    
  }



  template<typename T>
  void register_union()
  {
    namespace detail = foxtrot::detail;
    
    auto tp = rttr::type::get<T>();
    static auto nm = tp.get_name().to_string();
    rttr::registration::class_<T> reg(nm);

    reg(rttr::metadata("unionmeta", true),
	rttr::metadata("ft_type", variant_types::UNION_TYPE))
      .constructor()
      .method("get", &detail::union_get<T>)
      .method("held_type", &detail::union_held_type<T>);
      //      .method("possible_types", &detail::union_possible_types<T>);

    detail::variant_converter_reg<T>::reg();
  }


  rttr::variant union_get(const rttr::variant& var);
  std::vector<rttr::type> union_possible_types(const rttr::variant& var);
  rttr::type union_held_type(const rttr::variant& var);

}
