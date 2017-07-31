#include "stellarnet.h"
#include "ProtocolError.h"
#include "DeviceError.h"
#include <libusb.h>
#include <fstream>
#include <sstream>

#include <thread>
#include <chrono>

#define DEVICE_ID_ADDR 0x20
#define COEFF_C1_ADDR 0x80
#define COEFF_C2_ADDR 0xA0
#define COEFF_C3_ADDR 0xC0
#define COEFF_C4_ADDR 0xE0

#include <map>

const std::map<int,int> pixel_map
{
  {1 , 2048},
  {2 , 1024},
  {3 , 2048},
  {4 , 1024},
  {5 , 512},
  {6, 1024}  
};

auto reqtp_out = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
auto reqtp_in = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;

foxtrot::devices::stellarnet::stellarnet(const std::string& firmware_file, int timeout_ms): Device(nullptr),
_lg("stellarnet"), _firmware_file(firmware_file), _timeout_ms(timeout_ms)
{
  if( int ret = libusb_init(&_ctxt) < 0)
  {
   _lg.Error("error initializing libusb");
  throw  ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret))); 
  }
  
  //device list
  auto free_devlist = [] (libusb_device** list) { libusb_free_device_list(list, true);};
  _lg.Info("reading device list...");
  
  libusb_device** listptr;
  auto num_devs = libusb_get_device_list(_ctxt,&listptr);
  
  
  //run through and find any unloaded devices
    for(auto i =0; i< num_devs; i++)
    {
      _lg.Trace("getting device descriptor...");
      libusb_device_descriptor desc;
      if( int ret = libusb_get_device_descriptor(*(listptr+i), &desc) < 0)
      {
	_lg.Error("error getting device descriptor"); 
	throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
      };
      
      if(desc.idVendor == FX2_VID && desc.idProduct == FX2_PID )
      {
	_lg.Info("found empty FX2 chip, uploading firmware...");
	reenumerate_device(&desc,*(listptr+i));
	//NOTE: loop will run again and init real device
	break;
	
      }
    };
    libusb_free_device_list(listptr,true);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));


    
    num_devs = libusb_get_device_list(_ctxt,&listptr);
   bool dev_found = false;
    
    for(auto i =0; i< num_devs; i++)
    {
      _lg.Trace("getting device descriptor...");
      libusb_device_descriptor desc;
      if( int ret = libusb_get_device_descriptor(*(listptr+i), &desc) < 0)
      {
	_lg.Error("error getting device descriptor"); 
	throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
      };
      
      if(desc.idVendor == STELLARNET_VID && desc.idProduct == STELLARNET_PID)
      {
	_lg.Info("found enumerated stellarnet device!");
	setup_reenumerated_device(&desc, *(listptr+i));
	
	dev_found = true;
	break;
      }
      
    }
  
  if(!dev_found)
  {
      _lg.Error("no stellarnet device found...");
      libusb_free_device_list(listptr,true);
      throw DeviceError("no stellarnet device found!");
  }
         

}

foxtrot::devices::stellarnet::stellarnet(const foxtrot::parameterset& params)
: stellarnet(boost::get<std::string>(params.at("firmware_file")),boost::get<int>(params.at("timeout_ms")))
{

}



foxtrot::devices::stellarnet::~stellarnet()
{
  if(_hdl)
  {
    libusb_release_interface(_hdl,0);
    libusb_close(_hdl);
  }
  
  if(_ctxt)
  {
    libusb_exit(_ctxt);
  };

}


const std::string foxtrot::devices::stellarnet::getDeviceTypeName() const
{
  return "stellarnet";
}


void foxtrot::devices::stellarnet::reenumerate_device(libusb_device_descriptor* desc, libusb_device* dev)
{
  
  //set configuration
  if(int ret = libusb_open(dev,&_hdl) < 0)
  {
    _lg.Error("error opening device...");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }

  int cfg;
  if( int ret = libusb_get_configuration(_hdl,&cfg) < 0)
  {
    _lg.Error("error checking active config..>");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  if(cfg != 1)
  {
   libusb_set_configuration(_hdl,1); 
  }
  
  
  //load firmware
  
  //CPU into reset
  unsigned char reset_state[1] = {1};
  if(int ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,0xE600,0,reset_state,1,_timeout_ms) < 0)
  {
    _lg.Error("error putting CPU into reset");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  //load firmware
  _lg.Debug("firmware file: " + _firmware_file);
  std::ifstream ifs(_firmware_file);
  if(!ifs.is_open())
    {
      _lg.Error("can't open firmware file");
      throw ProtocolError("can't open firmware file");

    }

  std::string line;
  while(std::getline(ifs,line))
  {
    
    int count = std::stoi(line.substr(1,2),0,16);
    _lg.Trace("count: " + std::to_string(count));
    int addr = std::stoi(line.substr(3,4),0,16);
    _lg.Trace("addr: " + std::to_string(addr));
    
    auto payloadstr = line.substr(9,count*2);
    
    std::vector<unsigned char> payload;
    payload.resize(count);
     
    auto striter = payloadstr.begin();
    for(auto& c_out : payload)
    {
      
      c_out = std::stoi(std::string(striter, striter+2),0,16);
      striter+=2;
    };
    
    if(auto ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,addr,0,payload.data(),payload.size(),_timeout_ms) < 0)
    {
     _lg.Error("error loading firmware at address: " + std::to_string(addr));
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    }
    
  };
  
  //CPU out of reset
  reset_state[0] = 0;
  if(auto ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,0xE600,0,reset_state,1,_timeout_ms) < 0)
  {
    _lg.Error("error taking CPU out of reset");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  _lg.Info("firmware upload finished");
  

  // if(auto ret = libusb_reset_device(_hdl) <0 )
  //   {
  //     _lg.Error("error resetting USB device...");
      
  //   }



  libusb_close(_hdl);
  

}

void foxtrot::devices::stellarnet::setup_reenumerated_device(libusb_device_descriptor* desc, libusb_device* dev)
{
  
  auto ret = libusb_open(dev,&_hdl);
  if(ret <0)
  {
    _lg.Error("error opening device for setup");
    throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  if(int ret = libusb_claim_interface(_hdl,0) < 0)
  {
    _lg.Error("error releasing interface");
    throw ProtocolError(std::string("libusb error: " ) + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  
  
  _lg.Trace("getting detector type...");
  auto det_type = get_stored_bytes(COEFF_DEVICE_ID_ADDR);
  
  std::string dettypestr(det_type.begin(), det_type.end());
  _lg.Info("detector type:" + dettypestr);
  
  std::istringstream iss(dettypestr);
  iss >> _dettype;
  iss >> _devid;
  
  
  _lg.Trace("dettype:" + _dettype + "|");
  _lg.Trace("_devid:" +_devid + "|");
  
  _lg.Trace("getting lambda coefficients...");
  auto lambda_coeff_1 = get_stored_bytes(COEFF_C1_ADDR);
  auto lambda_coeff_2 = get_stored_bytes(COEFF_C2_ADDR);
  auto lambda_coeff_3 = get_stored_bytes(COEFF_C3_ADDR);
  auto lambda_coeff_4 = get_stored_bytes(COEFF_C4_ADDR);
  
  //TODO: convert coeffs properly to float
  
  
  _coeffs.resize(4);
  std::string coeffstr;
  
  iss.str(std::string(lambda_coeff_1.begin(), lambda_coeff_1.end()));
  iss >> coeffstr;
  _coeffs[0] = std::stod(coeffstr);
  
  iss.str(std::string(lambda_coeff_2.begin(), lambda_coeff_2.end()));
  iss >> coeffstr;
  _coeffs[1] = std::stod(coeffstr);
  
  
  iss.str(std::string(lambda_coeff_3.begin(), lambda_coeff_3.end()));
  iss >> coeffstr;
  _coeffs[2] = std::stod(coeffstr);
  
  
  iss.str(std::string(lambda_coeff_4.begin(), lambda_coeff_4.end()));
  iss >> coeffstr;
  _coeffs[3] = std::stod(coeffstr);
  
  _lg.Trace("setting default device timing...");
  set_device_timing(100,3);
  
  
  //find device type and setup pixel number
  _devidx = lambda_coeff_1[31] - static_cast<int>('0');
  
    
  
}

void foxtrot::devices::stellarnet::set_device_timing(unsigned short tm, unsigned char x_timing)
{
  unsigned char msb = tm >>8 ;
  unsigned char lsb = tm & 0xFF;
  
  unsigned char xt = 1 << (x_timing + 1) ;
  
  unsigned char msd = tm > 1 ? 0x1F : 0x0F;
  
  std::array<unsigned char,4 > data {msb,lsb,xt,msd};

  if(!_hdl)
  {
    throw DeviceError("device not open when trying to set timing!");
  }
  
  auto ret = libusb_control_transfer(_hdl,reqtp_out,0xB4,0,0,data.data(),data.size(),_timeout_ms);
  if(ret <0)
  {
    _lg.Error("set_device_timing error");
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
    
}

void foxtrot::devices::stellarnet::set_device_config(short unsigned integration_time, unsigned char XTiming, unsigned short boxcar_smooth, int tempcomp)
{
  if(integration_time < 2)
  {
    throw std::out_of_range("invalid integration time setting");
  };
  
  if(XTiming <1 || XTiming > 4)
  {
    throw std::out_of_range("invalid XTiming setting");
  
  }
  
  if(boxcar_smooth < 1)
  {
    throw std::out_of_range("invalid boxcar setting");
  };

  //TODO: set up everything...
  
  set_device_timing(integration_time, XTiming);
  
  
}

std::array< unsigned char, 0x20 > foxtrot::devices::stellarnet::get_stored_bytes(unsigned int addr)
{
  if(addr > 0x100 )
  {
    throw std::out_of_range("address too high!");
  }
  else if(addr % 0x20 != 0)
  {
   throw std::out_of_range("addess not correct stride"); 
  }
  
  //WARNING: is this single bytes??
  std::array<unsigned char,3> out_dat {0, addr, 0};
  
  if(!_hdl)
  {
    throw DeviceError("invalid device handle!");
  }
  
  auto ret = libusb_control_transfer(_hdl,reqtp_out,0xB6, 0,0, out_dat.data(),out_dat.size(),_timeout_ms);
  if(ret <0)
  {
    _lg.Error("set_device_timing error");
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  
  std::array<unsigned char, 0x21> indata;
  ret = libusb_control_transfer(_hdl,reqtp_in,0xB5, 0,0, indata.data(), indata.size(), _timeout_ms); 
  if(ret <0)
  {
    _lg.Error("set_device_timing error");
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  std::array<unsigned char, 0x20> out;
  std::copy(indata.begin() +1 , indata.end(), out.begin());
  
  return out;
  
  
}


std::vector< unsigned short> foxtrot::devices::stellarnet::read_spectrum(int int_time)
{
  if(!_hdl)
  {
    throw DeviceError("invalid device handle...");
  }
  
  set_device_timing(int_time,3);
  
  int timeout = (int_time + 500) ;
  _lg.Trace("beginning spectrum integration");
  
  char data[1] {0};
  auto ret = libusb_control_transfer(_hdl,reqtp_out,0xB2,0,0,nullptr,0,_timeout_ms);
  if(ret <0)
  {
    _lg.Error("begin spectrum error");
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
  }
  
  
  _lg.Trace("waiting for integration...");
  
  //check data available
  std::vector<unsigned short> specdata;
  specdata.resize(pixel_map.at(_devidx));
  
  int count_ms = 0;
  while(true)
  {
    _lg.Trace("checking if data ready...");
    unsigned char response[2];
    auto ret = libusb_control_transfer(_hdl,reqtp_in,0xB3,0,0,response,2,_timeout_ms);
    if(ret <0)
    {
    _lg.Error("check data ready error");
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    }
  
    
    if(response[1])
    {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    count_ms += 3;
    
    if(count_ms > timeout)
    {
      throw DeviceError("timeout reading spectrum");
    }
  };
  
  _lg.Trace("reading back spectrum data...");
  int actlen;
  ret = libusb_bulk_transfer(_hdl,0x88,reinterpret_cast<unsigned char*>(specdata.data()),specdata.size()*2,&actlen,_timeout_ms);
    if(ret <0)
    {
    _lg.Error("error reading back data");
     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
    }
  
  
  return specdata;

}


std::vector< double > foxtrot::devices::stellarnet::get_coeffs() const
{
  return _coeffs;
}





RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::stellarnet;

 registration::class_<stellarnet>("foxtrot::devices::stellarnet")
 .method("read_spectrum", &stellarnet::read_spectrum)
 ( parameter_names("int_time_ms"),
   metadata("streamdata",true)
   
 )
 .method("get_coeffs", &stellarnet::get_coeffs)
 (metadata("streamdata",true)
 )
 ;
  
  
  
  
}

