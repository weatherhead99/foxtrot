#pragma once
#include <random>
#include <vector>
#include <string>

#include <foxtrot/server/Device.h>

namespace foxtrot
{
    namespace devices
    {
      
      enum class dummyEnum {
        dummy_1,
        dummy_2
      };
      
      struct dummyStruct
      {
          std::string strval;
          unsigned uval;
          bool bval;
          double dval;
      };
      
      
    class dummyDevice : public foxtrot::Device 
    {
        RTTR_ENABLE(Device)
    public:
        dummyDevice();
	dummyDevice(const std::string& devcomment);
        
        double getRandomDouble() ;
	
        int getCounter();
        
        void setWomble(double val);
        
        double getWomble();
        
        void doNothing();
        
        void resetCounter();

        void brokenMethod();
        std::vector<int> unsupportedtype();
            
        int add(int a1, int a2);
            
        const std::string getDeviceTypeName() const override;
        
        void setInt(int a);
        int getInt() const;
        
        dummyEnum returns_custom_enum(int in);
        int takes_custom_enum(dummyEnum in);
        
        dummyStruct returns_custom_struct();
        
	
	std::vector<unsigned char> getCountStream(int n);
    std::vector<double> getRandomVector(int n);

    int longdurationmethod(int n_sec);
    
    protected:
        
        
    private:
        std::default_random_engine _generator;
        std::normal_distribution<double> _distribution;
        
        int _counter = 0;
        
        int _int = 0;
        double _womble = 0.0;
        
    };
    
    
    }
    
}


