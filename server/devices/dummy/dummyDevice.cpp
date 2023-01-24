#include <type_traits>
#include <iostream>
#include <thread>
#include <chrono>

#include <rttr/type>
#include <rttr/registration>

#include <foxtrot/DeviceError.h>
#include <any>

#include <foxtrot/devices/dummyDevice.h>
#include <foxtrot/ft_tuple_helper.hh>

// dummyDevice::dummyDevice() : Device(nullptr)
foxtrot::devices::dummyDevice::dummyDevice() : Device(nullptr)
{
}

foxtrot::devices::dummyDevice::dummyDevice(const std::string& devcomment): Device(nullptr, devcomment)
{

}


// int dummyDevice::getCounter()
int foxtrot::devices::dummyDevice::getCounter()
{
    return _counter++;
}


// double dummyDevice::getRandomDouble() 
double foxtrot::devices::dummyDevice::getRandomDouble()
{
//     std::cout << "generating random double.." << std::endl;
    auto num  = _distribution(_generator);
//     std::cout << "num: " << num << std::endl;
    return num;
}

// void dummyDevice::resetCounter()
void foxtrot::devices::dummyDevice::resetCounter()
{
    _counter = 0;
}


const std::string foxtrot::devices::dummyDevice::getDeviceTypeName() const
{
    return "dummyDevice";
}

int foxtrot::devices::dummyDevice::add(int a1, int a2)
{
    std::cout << "adding numbers: " << a1 << " and " << a2 << std::endl;
    return a1 + a2;
}

void foxtrot::devices::dummyDevice::brokenMethod()
{
    class DeviceError except("womble!");
    throw except;
}


std::any foxtrot::devices::dummyDevice::unsupportedtype()
{
    std::vector<int> out {1,2,3,4};
    return out;
}

std::vector< unsigned char > foxtrot::devices::dummyDevice::getCountStream(int n)
{
  std::vector<unsigned char> out;
  out.reserve(n);
  unsigned char o = 0;
  for(int i=0; i< n; i++)
  {  
    out.push_back(o++);
    
  }
  
  return out;

}


std::vector<double> foxtrot::devices::dummyDevice::getRandomVector(int n)
{
    std::vector<double> out;
    out.reserve(n);
    for(int i =0 ; i<n; i++)
    {
        out.push_back(getRandomDouble());
    }
    
    return out;
}

int foxtrot::devices::dummyDevice::longdurationmethod(int n_sec)
{
  std::this_thread::sleep_for(std::chrono::seconds(n_sec));

  return 0;
}

void foxtrot::devices::dummyDevice::setInt(int a)
{
    _int = a;
    
}

int foxtrot::devices::dummyDevice::getInt() const
{
    return _int;
    
}


void foxtrot::devices::dummyDevice::setWomble(double val)
{
    _womble = val;
};


double foxtrot::devices::dummyDevice::getWomble()
{
    return _womble;
}

int foxtrot::devices::dummyDevice::takes_custom_enum(foxtrot::devices::dummyEnum in)
{
  return 1;

}


foxtrot::devices::dummyEnum foxtrot::devices::dummyDevice::returns_custom_enum(int in)
{
  return dummyEnum::dummy_1;

}

std::string 
foxtrot::devices::dummyDevice::takes_custom_struct(const dummyStruct& in)
{
    return in.strval;
};


bool
foxtrot::devices::dummyDevice::takes_pointer_type(int* in)
{
    if(in)
        return true;
    return false;
}

foxtrot::devices::dummyStruct foxtrot::devices::dummyDevice::returns_custom_struct()
{
    dummyStruct out;
    out.strval = "hello";
    out.uval = 0xDEAD;
    out.bval = false;
    out.dval = 3.14159;
    return out;
}


std::tuple<int,std::string> foxtrot::devices::dummyDevice::returns_int_str_tuple()
{
    return std::make_tuple(0x1337,"hello");
}


std::tuple<double,int,double> foxtrot::devices::dummyDevice::returns_unregistered_tuple()
{
    return std::make_tuple(3.14,2,5.18);
}


std::pair<int, double> foxtrot::devices::dummyDevice::returns_pair()
{
    return std::make_pair(1,1.0);
}

void foxtrot::devices::dummyDevice::doNothing()
{
};

std::array<unsigned char, 5> foxtrot::devices::dummyDevice::returns_std_array()
{
    std::array<unsigned char, 5> out ={1,2,3,4,5};
    return out;
    
}

std::array<int, 3> foxtrot::devices::dummyDevice::returns_std_int_array()
{
    std::array<int, 3> out = {1567, 1568, 1577};
    return out;
}


foxtrot::devices::arrStruct foxtrot::devices::dummyDevice::returns_struct_std_array() 
{
    arrStruct out;
    
    out.boom2 = returns_std_array();
    out.boom=12;
    return out;

    
}




RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::dummyDevice;
 registration::class_<dummyDevice>("foxtrot::devices::dummyDevice")
 .property_readonly("getCounter", &dummyDevice::getCounter)
 .property_readonly("getRandomDouble", &dummyDevice::getRandomDouble)
 .method("resetCounter", &dummyDevice::resetCounter)
 .method("add", &dummyDevice::add)
 (
     parameter_names("a1","a2")
     )
 .method("brokenMethod",&dummyDevice::brokenMethod)
 .property_readonly("unsupportedtype",&dummyDevice::unsupportedtype)
 .method("getCountStream",&dummyDevice::getCountStream)
 (
   parameter_names("n"),
  metadata("streamdata",true)
   )
 .method("getRandomVector",&dummyDevice::getRandomVector)
 (
     parameter_names("n"),
     metadata("streamdata",true)
     )
 .method("longdurationmethod",&dummyDevice::longdurationmethod)
 ( parameter_names("n_sec")
 )
 .property("Int",&dummyDevice::getInt, &dummyDevice::setInt)
 (parameter_names("a"))
 .method("setWomble",&dummyDevice::setWomble)
 (parameter_names("val"))
 .method("getWomble", &dummyDevice::getWomble)
 .method("takes_custom_enum", &dummyDevice::takes_custom_enum)
 .method("returns_custom_enum", &dummyDevice::returns_custom_enum)
 .method("doNothing", &dummyDevice::doNothing)
 .method("returns_custom_struct", &dummyDevice::returns_custom_struct)
 .method("takes_pointer_type", &dummyDevice::takes_pointer_type)
 .method("takes_custom_struct", &dummyDevice::takes_custom_struct)
 .method("returns_int_str_tuple", &dummyDevice::returns_int_str_tuple)
 .method("returns_pair", &dummyDevice::returns_pair)
 .method("returns_unregistered_tuple", &dummyDevice::returns_unregistered_tuple)
 .method("returns_std_array", &dummyDevice::returns_std_array)
 .method("returns_struct_std_array", &dummyDevice::returns_struct_std_array)
 .method("returns_std_int_array", &dummyDevice::returns_std_int_array);
 
 foxtrot::register_tuple<std::pair<int,double>>();
 foxtrot::register_tuple<std::tuple<int,std::string>>();
 
 using foxtrot::devices::dummyEnum;
 using foxtrot::devices::dummyStruct;
 using foxtrot::devices::arrStruct;
 
 registration::class_<dummyStruct>("foxtrot::devices::dummyStruct")
 .constructor()(policy::ctor::as_object)
 .property("strval", &dummyStruct::strval)
 .property("uval", &dummyStruct::uval)
 .property("bval", &dummyStruct::bval)
 .property("dval", &dummyStruct::dval);
 
 registration::class_<arrStruct>("foxtrot::devices::arrStruct")
 .constructor()(policy::ctor::as_object)
 .property("boom", &arrStruct::boom)
 .property("boom2", &arrStruct::boom2);
 
 
 registration::enumeration<dummyEnum>("foxtrot::devices::dummyEnum")
 (
     value("dummy_1", dummyEnum::dummy_1),
     value("dummy_2", dummyEnum::dummy_2)
     );
 
 
}
