#pragma once
#include <string>
#include <sstream>
#include <rttr/type>

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
         
         void setSource(int ch, gpio_source& source);
         gpio_source getSource(int ch);
         
         
     protected:
         archonGPIO(archon& arch, int modpos);
         
         
     private:
         archon& _arch_gpio;
         int _modpos_gpio;
         std::ostringstream _oss;
         
         void writeConfigKey_gpio(const string& key, const string& val);
	    string readConfigKey_gpio(const string& key);
	    
         
         
         
         
     };
        
        
    }


}
