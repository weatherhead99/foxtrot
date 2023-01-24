#include <rttr/type>
#include <iostream>
#include <foxtrot/typeUtil.h>
#include <foxtrot/Logging.h>

using std::cout;
using std::endl;




int main()
{
    int a = -12;
    
    foxtrot::setLogFilterLevel(sl::trace);
    
    rttr::variant v = a;
    foxtrot::Logging lg("byte_convert_play");
    auto simpvar = foxtrot::get_simple_variant_wire_type(a, &lg);
//     
//     cout << "can convert to unsigned: " << v.can_convert<unsigned>() << endl;
//     cout << "can convert to unsigned short: " << v.can_convert<unsigned short>() << endl;
//     cout << "can convert to unsigned char: " << v.can_convert<unsigned char>() << endl;
//     
//     bool ok = false;
//     auto b = v.convert<unsigned short>(&ok);
//     cout << "b: " << b << endl;
//     cout << "ok: " << (int) ok << endl;
    


}
