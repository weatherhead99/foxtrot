#include <type_traits>
#include <iostream>
#include <thread>
#include <chrono>

#include <rttr/type>
#include <rttr/registration>

#include <foxtrot/DeviceError.h>

#include <foxtrot/devices/dummyDevice.h>

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


std::vector<int> foxtrot::devices::dummyDevice::unsupportedtype()
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

foxtrot::devices::dummyStruct foxtrot::devices::dummyDevice::returns_custom_struct()
{
    dummyStruct out;
    out.strval = "hello";
    out.uval = 0xDEAD;
    out.bval = false;
    out.dval = 3.14159;
    return out;
}




void foxtrot::devices::dummyDevice::doNothing()
{
};


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
 ;
 
 using foxtrot::devices::dummyEnum;
 using foxtrot::devices::dummyStruct;
 
 registration::class_<dummyStruct>("foxtrot::devices::dummyStruct")
 .property("strval", &dummyStruct::strval)
 .property("uval", &dummyStruct::uval)
 .property("bval", &dummyStruct::bval)
 .property("dval", &dummyStruct::dval);
 
 
 registration::enumeration<dummyEnum>("foxtrot::devices::dummyEnum")
 (
     value("dummy_1", dummyEnum::dummy_1),
     value("dummy_2", dummyEnum::dummy_2)
     );
 
 
}
