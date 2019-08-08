#include <iostream>
#include <foxtrot/ft_tuple_helper.hh>
#include <rttr/registration>
#include <tuple>
#include <string>


using tupleT = std::tuple<int,std::string>;

RTTR_REGISTRATION
{
    foxtrot::register_tuple<tupleT>();    
    
}

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    auto tp = rttr::type::get<tupleT>();
    
    auto sz = foxtrot::tuple_size(tp);
    cout << "size: " << sz << endl;
    
    
    
}
