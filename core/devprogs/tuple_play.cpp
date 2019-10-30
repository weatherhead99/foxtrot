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
    tuple_descriptor desc = foxtrot::describe_tuple(tp,&lg);
    
    cout << "description: " << desc.DebugString() << endl;
    
    
    
    std::ofstream ofs("test.out");
    desc.SerializeToOstream(&ofs);
    
}
