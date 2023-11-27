#include <iostream>
#include <foxtrot/ft_tuple_helper.hh>
#include <foxtrot/typeUtil.h>
#include <rttr/registration>
#include <tuple>
#include <string>
#include <foxtrot/Logging.h>
#include <fstream>

using tupleT = std::tuple<int,std::string>;

RTTR_REGISTRATION
{
    foxtrot::register_tuple<tupleT>();    
    
}

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    foxtrot::setLogFilterLevel(sl::trace);
    
    auto tp = rttr::type::get<tupleT>();
    
    auto sz = foxtrot::tuple_size(tp);
    cout << "size: " << sz << endl;
    
    foxtrot::Logging lg("tuple_play");
    auto desc = foxtrot::describe_tuple(tp,&lg);
    
    cout << "description: " << desc.DebugString() << endl;
    

    std::vector<tupleT> vec;
    vec.push_back( std::make_tuple(2, std::string{"bobsyouruncle"}));

    
    

    rttr::variant v1 = std::make_tuple(1, std::string{"hello"});

    cout << "match typs: " << (int) (v1.get_type() == tp) << endl;
    cout << "tp direct: " << tp.get_name() << endl;
    cout << "tp indirect: " << v1.get_type().get_name() << endl;

    
    auto sz2 = foxtrot::tuple_size(v1.get_type());
    cout << "size: " << sz2 << endl;
    

    auto wiretp1= foxtrot::get_tuple_wire_type(v1, &lg); 
    

    rttr::variant v2 = vec;
    auto view = v2.create_sequential_view();

    auto inner_tp = view.get_rank_type(1);
    cout << "rank 1 type: " << inner_tp.get_name() << endl;
    cout << "sz from rank 1 tp: "  << foxtrot::tuple_size(inner_tp) << endl;
    

    auto innerwiretp1 = foxtrot::get_variant_wire_type(v2, &lg, false);
    cout << "innerwiretp1:" << innerwiretp1.DebugString() << endl;

    auto innerwiretp2 = foxtrot::get_variant_wire_type(v2, nullptr, true);
    cout << "innerwiretp2:" << innerwiretp2.DebugString() << endl;

    
    
    std::ofstream ofs("test.out");
    desc.SerializeToOstream(&ofs);
    
}
