#include <foxtrot/ft_union_helper.hh>

#include <variant>
#include <string>
#include <iostream>


using Var = std::variant<int, std::string>;

int main() { Var v = "hello";

  rttr::variant v2 = v;  
  std::cout <<"held type: " <<  v2.get_type().get_name() << std::endl;
  
  
  auto targs = v2.get_type().get_template_arguments();
  for(auto& t : targs)
    std::cout << t.get_name() << ",";;
  std::cout << std::endl;


  auto meth = v2.get_type().get_method("get");
  std::cout << "meth is valid? " << (int) meth.is_valid() << std::endl;
  
  auto v3 = meth.invoke(rttr::instance(), v2);


  auto v4 = v2.get_type().get_method("held_type").invoke(rttr::instance(), v2).get_value<rttr::type>();

  std::cout << v4.get_name() << std::endl;
  std::cout << v3.get_type().get_name() << std::endl;
  std::cout << "value:" << v3.to_string() << std::endl;

  auto v5 = rttr::variant{std::string("hello")};
  std::cout << v5.can_convert<Var>() << std::endl;


  auto postypes = foxtrot::union_possible_types(v2);

  for(auto& tp: postypes)
    std::cout << tp.get_name() << ",";
  std::cout << std::endl;



  
  }


RTTR_REGISTRATION
{

  foxtrot::register_union<Var>();

}
