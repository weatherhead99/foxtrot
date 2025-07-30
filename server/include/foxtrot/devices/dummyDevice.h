#pragma once
#include <random>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <any>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <chrono>

#include <foxtrot/Device.h>
#include <foxtrot/server/DeviceLocks.hh>

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
      
      struct arrStruct
      {
          int boom;
          std::array<unsigned char, 5> boom2;
          
      };
      
      
    class dummyDevice : public virtual foxtrot::Device, public  foxtrot::GlobalDeviceLock<foxtrot::Device>
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
        std::any remoteBindTypeAny();

      std::shared_ptr<std::string> remoteBindTypePointer(const std::string& inp);
            
        int add(int a1, int a2);
            
        const std::string getDeviceTypeName() const override;
        
        void setInt(int a);
        int getInt() const;
        
        dummyEnum returns_custom_enum(int in);
        int takes_custom_enum(dummyEnum in);

      std::string  takes_remote_obj(std::shared_ptr<std::string> ptr);
      
        std::string takes_custom_struct(const dummyStruct& in);
        
        bool takes_pointer_type(int* in=nullptr);
        
        dummyStruct returns_custom_struct();
        
        std::tuple<int,std::string> returns_int_str_tuple();
        std::tuple<double,int,double> returns_unregistered_tuple();
        std::pair<int,double> returns_pair();
        
        
	std::vector<unsigned char> getCountStream(int n);
    std::vector<double> getRandomVector(int n);

      std::vector<double> returns_nonstream_double_vector();
      std::vector<std::string> returns_nonstream_string_vector();
      
    std::array<unsigned char, 5> returns_std_array();
    std::array<int,3 > returns_std_int_array();
    arrStruct returns_struct_std_array();

    int longdurationmethod(int n_sec);

      //Union test methods
      std::variant<int, double, std::string> returns_variant(int i);
      std::variant<double, unsigned> returns_unregistered_variant();
      int takes_variant(std::variant<int, double, std::string> var);
      std::variant<int,double,std::string> takes_tuple(const std::tuple<int,double,std::string>& in);
      
      bool takes_optional(std::optional<int> opt);
      int get_last_supplied_optional_value() const;
      std::optional<int> returns_optional(int val, bool ret);


      //test methods for repeated names
      bool methodNamedSomethingSilly();
      bool methodNamedSomethingSilly(bool arg);


      //test methods for returning dictionaries/maps
      std::map<string, int> returns_strintmap() const;
      std::map<int, string> returns_intstrmap() const;

      std::unordered_map<string, string> returns_unorderedmap() const;

      std::chrono::time_point<std::chrono::system_clock> returns_current_time() const;

    protected:
        
        
    private:
        std::default_random_engine _generator;
        std::normal_distribution<double> _distribution;
        
        int _counter = 0;
        
        int _int = 0;
        double _womble = 0.0;

      int _lastopt = 0;
        
    };
    
    
    }
    
}


