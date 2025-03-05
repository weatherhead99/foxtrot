#include <type_traits>
#include <iostream>
#include <thread>
#include <chrono>

#include <type_traits>

#include <rttr/type>
#include <rttr/registration>

#include <foxtrot/DeviceError.h>
#include <any>

#include <foxtrot/devices/dummyDevice.h>
#include <foxtrot/ft_tuple_helper.hh>
#include <foxtrot/ft_union_helper.hh>

#include <foxtrot/ft_optional_helper.hh>

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


std::any foxtrot::devices::dummyDevice::remoteBindTypeAny()
{
    std::vector<int> out {1,2,3,4};
    return out;
}

std::shared_ptr<std::string> foxtrot::devices::dummyDevice::remoteBindTypePointer(const std::string& inp)
{
  std::ostringstream oss;
  oss << "ok then, " << inp << "!";
  
  return std::make_unique<std::string>(oss.str());
 
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

std::string foxtrot::devices::dummyDevice::takes_remote_obj(
    std::shared_ptr<std::string> ptr)
{
  return *ptr;
};


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


std::variant<int,double,std::string> foxtrot::devices::dummyDevice::returns_variant(int i)
{

  std::variant<int,double,std::string> out;
  
  switch(i)
    {
    case(0):
      out = 12; break;
    case(1):
      out = 7.253; break;
    case(2): 
      out = "badabingbadaboom"; break;
    default:
      throw std::out_of_range("invalid argument");
    }

  return out;
}

std::variant<double, unsigned> foxtrot::devices::dummyDevice::returns_unregistered_variant()
{
  std::variant<double, unsigned> out = 1.0;
  return out;
}

int foxtrot::devices::dummyDevice::takes_variant(std::variant<int, double, std::string> var)
{
  return std::visit( [] (auto& arg)
  {
    using T = std::decay_t<decltype(arg)>;
    if constexpr(std::is_same_v<T, int>)
      return 0;
    else if constexpr(std::is_same_v<T, double>)
      return 1;
    else if constexpr(std::is_same_v<T, std::string>)
      return 2;
  }

 , var);

}


std::vector<double> foxtrot::devices::dummyDevice::returns_nonstream_double_vector()
{
  return {1.0, 2.0, 3.14159, 4.0, 5.0};
}

std::vector<std::string> foxtrot::devices::dummyDevice::returns_nonstream_string_vector()
{
  return { "red", "yellow" , "green"};
}


std::variant<int, double, std::string> foxtrot::devices::dummyDevice::takes_tuple(const std::tuple<int, double, std::string>& in)
{
  return std::get<2>(in);
}

bool foxtrot::devices::dummyDevice::takes_optional(std::optional<int> opt)
{
  if(opt.has_value())
    {
      _lastopt = *opt;
      return true;
    }

  return false;
  
}

std::optional<int> foxtrot::devices::dummyDevice::returns_optional(int val, bool ret)
{
  if(!ret)
    return std::nullopt;

  return val;
}

int foxtrot::devices::dummyDevice::get_last_supplied_optional_value() const
{
  return _lastopt;
}


bool foxtrot::devices::dummyDevice::methodNamedSomethingSilly()
{ return true;}

bool foxtrot::devices::dummyDevice::methodNamedSomethingSilly(bool arg)
{ return arg;}



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
   .property_readonly("remoteBindTypeAny", &dummyDevice::remoteBindTypeAny)
   .method("remoteBindTypePointer", &dummyDevice::remoteBindTypePointer)
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
 .method("returns_std_int_array", &dummyDevice::returns_std_int_array)
 .method("takes_remote_obj", &dummyDevice::takes_remote_obj)
   .method("returns_variant", &dummyDevice::returns_variant)(parameter_names("n"))
   .method("returns_unregistered_variant", &dummyDevice::returns_unregistered_variant)
   .method("takes_variant", &dummyDevice::takes_variant)(parameter_names("var"))
   .method("takes_tuple", &dummyDevice::takes_tuple)(parameter_names("in"))
   .property_readonly("last_supplied_optional_value", &dummyDevice::get_last_supplied_optional_value)
   .method("takes_optional", &dummyDevice::takes_optional)(parameter_names("opt"))
   .method("returns_optional", &dummyDevice::returns_optional)(parameter_names("val","ret"))
   .method("returns_nonstream_double_vector", &dummyDevice::returns_nonstream_double_vector)
   .method("returns_nonstream_string_vector", &dummyDevice::returns_nonstream_string_vector)
   .method("methodNamedSomethingSilly", select_overload<bool()>(&dummyDevice::methodNamedSomethingSilly))
   .method("methodNamedSomethingSilly", select_overload<bool(bool)>(&dummyDevice::methodNamedSomethingSilly));
   
 
 foxtrot::register_tuple<std::tuple<int, double, std::string>>;

 using Var = std::result_of< decltype(&dummyDevice::returns_variant)(dummyDevice, int)>::type;
 foxtrot::register_union<Var>();
 
 foxtrot::register_tuple<std::pair<int,double>>();
 foxtrot::register_tuple<std::tuple<int,std::string>>();
 foxtrot::register_tuple<std::tuple<int, double ,std::string>>();
 
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
