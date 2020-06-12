#include <foxtrot/server/auth_layer/SASLAuthProvider.hh>
#include <foxtrot/server/auth_layer/AuthBase.hh>
#include <iostream>


using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    cout << "hello"  << endl;
    
    std::unique_ptr <foxtrot::UserProviderInterface> userprovider{nullptr};
    
    foxtrot::UserAuthInterface auth_iface(std::move(userprovider));
    auto saslprov = std::make_shared<foxtrot::SASLAuthProvider>();
    auth_iface.add_provider(saslprov);
    
    
    for(auto& mech : auth_iface.get_supported_mechanisms())
    {
        cout << mech  << ",";
    }
    
    cout << endl;
        
    
}
