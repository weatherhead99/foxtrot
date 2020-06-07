#include <foxtrot/server/auth_layer/SASLAuthProvider.hh>
#include <gsasl.h>
#include <gcrypt.h>
#include <sstream>
#include <algorithm>
#include <iterator>

const std::vector<string> ft_SASL_mechanisms_supported = {"SCRAM-SHA-1", "GS2-KRB5"};

//TODO: channel bindings....
namespace foxtrot 
{
 
    SASLAuthProvider::SASLAuthProvider()
    : lg("SASLAuthProvider")
    {
        setlocale(LC_ALL, "");
        
        if(!gcry_check_version(GCRYPT_VERSION))
            throw std::logic_error("gcrypt version mismatch!");
        
        if(!gsasl_check_version(GSASL_VERSION))
            throw std::logic_error("gsasl version mismatch!");
        
        gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
        gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
        
        auto ret = gsasl_init(&ctx);
        if(ret != GSASL_OK)
        {
            string err_msg = "gsasl init failure: " + std::to_string(ret) + "  " + gsasl_strerror(ret);
            throw std::runtime_error(err_msg);
        }
        
        
    };
    
    SASLAuthProvider::~SASLAuthProvider()
    {
        gsasl_done(ctx);
    };
    
    std::vector<string> SASLAuthProvider::get_supported_mechanisms()
    {
        
        char* all_mechs = nullptr;
        
        gsasl_server_mechlist(ctx, &all_mechs);
        auto all_mechs_str = string(all_mechs);
        
        if(all_mechs)
            delete all_mechs;
        
        std::vector<string> out;
        lg.strm(sl::debug) << "all mechanisms reported by gsasl: " << all_mechs_str;
        
        std::istringstream iss(all_mechs_str);
        
        std::copy(std::istream_iterator<string>(iss),
                  std::istream_iterator<string>(),
                  std::back_inserter(out));
        
        auto mechit = out.begin();
        while(mechit != out.end())
        {
            auto it = std::find(ft_SASL_mechanisms_supported.begin(),
                                ft_SASL_mechanisms_supported.end(),
                                *mechit);
            
            if(it == ft_SASL_mechanisms_supported.end())
            {
                lg.strm(sl::trace) << "mechamism: " << *mechit << "not found in ft supported choices";
                out.erase(mechit);
                mechit = out.begin();
            }
            else
            {
                mechit++;
            }
        };
        
        
        return out;
    }
    
    
    
}
