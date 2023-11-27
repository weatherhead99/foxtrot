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
    


    rttr::variant v1 = std::make_tuple(1, std::string{"hello"});

    cout << "match typs: " << (int) (v1.get_type() == tp) << endl;
    cout << "tp direct: " << tp.get_name() << endl;
    cout << "tp indirect: " << v1.get_type().get_name() << endl;

    
    auto sz2 = foxtrot::tuple_size(v1.get_type());
    cout << "size: " << sz2 << endl;
    

    auto wiretp1= foxtrot::get_tuple_wire_type(v1, &lg); 
    
    
    
    std::ofstream ofs("test.out");
    desc.SerializeToOstream(&ofs);
    
}
