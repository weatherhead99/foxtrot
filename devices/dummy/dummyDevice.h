#pragma once
#include "Device.h"

#include <random>

#include <rttr/type>
#include <vector>


namespace foxtrot
{
    namespace devices
    {
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
        
        
        
        void resetCounter();

        void brokenMethod();
        std::vector<int> unsupportedtype();
            
        int add(int a1, int a2);
            
        const std::string getDeviceTypeName() const override;
        
        void setInt(int a);
        int getInt() const;
        
        
        
	
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


