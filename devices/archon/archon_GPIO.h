#pragma once
#include <string>
#include <sstream>
#include <rttr/type>
#include "archon_module_mapper.hh"

using std::string;

namespace foxtrot
{
    namespace devices
    {
        class archon;
        
        
        enum class gpio_source : short unsigned
        {
            low = 0,
            high = 1,
            clocked = 2,
            VCPU = 3,
        };
        
     class archonGPIO 
     {
         RTTR_ENABLE();
     public:
       void setLabel(int ch, const std::string& label);
         std::string getLabel(int ch);
         
         bool getDIOPower();
         void setDIOPower(bool onoff);
         
         void setDirection(int pair, bool inout);
         bool getDirection(int pair);
         
         void setSource(int ch, gpio_source source);
         gpio_source getSource(int ch);
         
         
     protected:
       archonGPIO(std::weak_ptr<archon>& arch, const archon_module_info& inf, bool pairwise_direction=true);
         
         
     private:

       string get_dirstring(int pairorchannel);
       
       std::weak_ptr<archon> _arch_gpio;
       archon_module_info _inf;
       
         std::ostringstream _oss;
       bool _pairwise_direction;
         
         void writeConfigKey_gpio(const string& key, const string& val);
	    string readConfigKey_gpio(const string& key);
	    
         
         
         
         
     };
        
        
    }


}
