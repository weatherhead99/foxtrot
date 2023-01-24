#include <rttr/type>
#include <iostream>
#include <foxtrot/typeUtil.h>
#include <foxtrot/Logging.h>

#include <boost/hana/set.hpp>
#include <boost/hana/contains.hpp>

using std::cout;
using std::endl;




int main()
{
    int a = -12;
    
    foxtrot::setLogFilterLevel(sl::trace);
    
    auto set = boost::hana::make_set(boost::hana::type_c<int>, boost::hana::type_c<double>);
    
    if constexpr(boost::hana::contains(set, boost::hana::type_c<int>) == boost::hana::true_())
    {
        cout << "yes!" << endl;
    }
    
    
    rttr::variant v = 3.14;
    foxtrot::Logging lg("byte_convert_play");
    auto simpvar = foxtrot::get_simple_variant_wire_type(v, &lg);
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
