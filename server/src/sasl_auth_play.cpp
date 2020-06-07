#include <foxtrot/server/auth_layer/SASLAuthProvider.hh>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    cout << "hello"  << endl;
    
    
    foxtrot::SASLAuthProvider prov;
    
    
    auto mechlist = prov.get_supported_mechanisms();
    
    for(const auto& mech: mechlist)
    {
        cout << mech << ",";
    }
    
    cout << endl;
    
    
}
