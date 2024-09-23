#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include <libusb.h>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/DeviceError.h>

#include "stellarnet.h"

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


using namespace foxtrot::devices;

Firmware foxtrot::devices::load_stellarnet_firmware_payload(const string& firmware_file)
{
   //check on the file first

  std::ifstream ifs(firmware_file);
  if(!ifs.is_open())
    throw foxtrot::DeviceError("couldn't open Stellarnet firmware file!");

  Firmware out;

  std::string line;
  while(std::getline(ifs,line))
  {

    int count = std::stoi(line.substr(1,2),0,16);
    unsigned addr = std::stoi(line.substr(3,4),0,16);

    auto payloadstr = line.substr(9,count*2);

    std::vector<unsigned char> payload;
    payload.resize(count);

    auto striter = payloadstr.begin();
    for(auto& c_out : payload)
    {
      c_out = std::stoi(std::string(striter, striter+2),0,16);
      striter+=2;
    };

    out[addr] = payload;

  }

  return out;
}


void foxtrot::devices::upload_stellarnet_device_firmware(LibUsbDevice& dev, const Firmware& fw, std::chrono::milliseconds timeout)
{
  foxtrot::Logging lg("stellarnet_firmware_loader");

  if(!dev.is_open())
    dev.open();

  if(not (dev.configuration() == 1))
    dev.set_configuration(1);

  //put FX2 device into reset
  const std::uint8_t FX2_RESET_bRequest = 0xA0;
  const std::uint16_t FX2_RESET_wValue = 0xE600;
  const std::uint16_t FX2_RESET_wIndex = 0x0;

  std::array<unsigned char, 1> reset_data = {1};

  lg.strm(sl::info) << "putting FX2 device into reset state...";
  dev.blocking_control_transfer_send(reqtp_out, FX2_RESET_bRequest,
				     FX2_RESET_wValue, FX2_RESET_wIndex, reset_data, timeout);


  lg.strm(sl::info) << "loading firmware payload...";
  for(auto [addr, payload] : fw)
    {
      try{
	dev.blocking_control_transfer_send(reqtp_out, FX2_RESET_bRequest, addr, 0, payload, timeout);
      }
      catch(foxtrot::protocols::LibUsbError& err)
	{
	  lg.strm(sl::error) << "error loading firmware at address: " << addr;
	  throw foxtrot::ProtocolError(err.what());
	}
    }

  //take CPU out of reset
  reset_data[0] = 0;
  dev.blocking_control_transfer_send(reqtp_out, FX2_RESET_bRequest,
				     FX2_RESET_wValue, FX2_RESET_wIndex, reset_data, timeout);


  lg.strm(sl::info) << "firmware upload should be finished!";
  dev.close();
}



stellarnet::stellarnet(const std::string& firmware_file, int timeout_ms): Device(nullptr),
_lg("stellarnet"),  _timeout_ms(timeout_ms)
{

  //scope this, so that the device gets freed when unloaded_dev disappears
  {
    auto unloaded_dev = foxtrot::protocols::find_single_device(FX2_VID, FX2_PID);
    if(!unloaded_dev.has_value())
      {
      _lg.strm(sl::info) << ("no unloaded FX2 device found, will proceed with stellarnet device finding...");
      }
    else
      {
	auto tm_ms = std::chrono::milliseconds(timeout_ms);
	auto fw = load_stellarnet_firmware_payload(firmware_file);
	upload_stellarnet_device_firmware(*unloaded_dev, fw, tm_ms);
	_lg.strm(sl::info) << "sleeping for device to come back";
	std::this_thread::sleep_for(std::chrono::milliseconds(800));
      }
  }


  auto loaded_dev = foxtrot::protocols::find_single_device(STELLARNET_VID, STELLARNET_PID);
  if(!loaded_dev.has_value())
    throw DeviceError("couldn't find firmware loaded Stellarnet device!");


  setup_stellarnet_device(*loaded_dev);
  _dev = std::make_unique<foxtrot::devices::LibUsbDevice>(*loaded_dev);




}



stellarnet::stellarnet(const foxtrot::parameterset& params)
: stellarnet(std::get<std::string>(params.at("firmware_file")),std::get<int>(params.at("timeout_ms")))
{

}


void stellarnet::setup_stellarnet_device(LibUsbDevice& dev)
{
  try
    {
      if(!dev.is_open())
	dev.open();

      dev.claim_interface(0);
    }
  catch(foxtrot::protocols::LibUsbError& err)
    {
      throw foxtrot::DeviceError(err.what());
    }

  auto det_type = get_stored_bytes(COEFF_DEVICE_ID_ADDR);
  std::string dettypestr(det_type.begin(), det_type.end());
  _lg.strm(sl::info) << "detector type: " << dettypestr;

  _lg.Trace("loading lambda coefficients");

  //first one is special, it also contains "device index";
  auto coeff_bytes = get_stored_bytes(COEFF_C1_ADDR);


  _coeffs[0] = bytes_to_coeff(coeff_bytes);
  _coeffs[1] = get_stored_coefficient(COEFF_C2_ADDR);
  _coeffs[2] = get_stored_coefficient(COEFF_C3_ADDR);
  _coeffs[3] = get_stored_coefficient(COEFF_C4_ADDR);


  //WARNING: this is probably total BULLSHIT!
  set_device_timing(100,3);
  _devidx = coeff_bytes[31] - static_cast<int>('0');

}


const std::string foxtrot::devices::stellarnet::getDeviceTypeName() const
{
  return "stellarnet";
}


// void foxtrot::devices::stellarnet::reenumerate_device(libusb_device_descriptor* desc, libusb_device* dev)
// {

//   //set

//   int ret = libusb_open(dev,&_hdl);
//   if( ret < 0)
//   {
//     _lg.Error("error opening device...");
//     _lg.strm(sl::error) << "error code: " << ret;
//     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
//   }

//   int cfg;
//   ret = libusb_get_configuration(_hdl, &cfg);
//   if( ret < 0)
//   {
//     _lg.Error("error checking active config..>");
//     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
//   }

//   if(cfg != 1)
//   {
//    libusb_set_configuration(_hdl,1);
//   }


//   //load firmware

//   //CPU into reset
//   unsigned char reset_state[1] = {1};
//   if(int ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,0xE600,0,reset_state,1,_timeout_ms) < 0)
//   {
//     _lg.Error("error putting CPU into reset");
//     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
//   }

//   //load firmware
//   _lg.Debug("firmware file: " + _firmware_file);
//   std::ifstream ifs(_firmware_file);
//   if(!ifs.is_open())
//     {
//       _lg.Error("can't open firmware file");
//       throw ProtocolError("can't open firmware file");

//     }

//   std::string line;
//   while(std::getline(ifs,line))
//   {

//     int count = std::stoi(line.substr(1,2),0,16);
//     _lg.Trace("count: " + std::to_string(count));
//     int addr = std::stoi(line.substr(3,4),0,16);
//     _lg.Trace("addr: " + std::to_string(addr));

//     auto payloadstr = line.substr(9,count*2);

//     std::vector<unsigned char> payload;
//     payload.resize(count);

//     auto striter = payloadstr.begin();
//     for(auto& c_out : payload)
//     {

//       c_out = std::stoi(std::string(striter, striter+2),0,16);
//       striter+=2;
//     };

//     if(auto ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,addr,0,payload.data(),payload.size(),_timeout_ms) < 0)
//     {
//      _lg.Error("error loading firmware at address: " + std::to_string(addr));
//      throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
//     }

//   };

//   //CPU out of reset
//   reset_state[0] = 0;
//   if(auto ret = libusb_control_transfer(_hdl,reqtp_out,0xA0,0xE600,0,reset_state,1,_timeout_ms) < 0)
//   {
//     _lg.Error("error taking CPU out of reset");
//     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
//   }

//   _lg.Info("firmware upload finished");


//   // if(auto ret = libusb_reset_device(_hdl) <0 )
//   //   {
//   //     _lg.Error("error resetting USB device...");

//   //   }



//   libusb_close(_hdl);


// }

// void foxtrot::devices::stellarnet::setup_reenumerated_device(libusb_device_descriptor* desc, libusb_device* dev)
// {

//   auto ret = libusb_open(dev,&_hdl);
//   if(ret <0)
//   {
//     _lg.Error("error opening device for setup");
//     throw ProtocolError(std::string("libusb error: ") + libusb_strerror(static_cast<libusb_error>(ret)));
//   }

//   if(int ret = libusb_claim_interface(_hdl,0) < 0)
//   {
//     _lg.Error("error releasing interface");
//     throw ProtocolError(std::string("libusb error: " ) + libusb_strerror(static_cast<libusb_error>(ret)));
//   }



//   _lg.Trace("getting detector type...");
//   auto det_type = get_stored_bytes(COEFF_DEVICE_ID_ADDR);

//   std::string dettypestr(det_type.begin(), det_type.end());
//   _lg.Info("detector type:" + dettypestr);

//   std::istringstream iss(dettypestr);
//   iss >> _dettype;
//   iss >> _devid;


//   _lg.Trace("dettype:" + _dettype + "|");
//   _lg.Trace("_devid:" +_devid + "|");

//   _lg.Trace("getting lambda coefficients...");
//   auto lambda_coeff_1 = get_stored_bytes(COEFF_C1_ADDR);
//   auto lambda_coeff_2 = get_stored_bytes(COEFF_C2_ADDR);
//   auto lambda_coeff_3 = get_stored_bytes(COEFF_C3_ADDR);
//   auto lambda_coeff_4 = get_stored_bytes(COEFF_C4_ADDR);

//   //TODO: convert coeffs properly to float


//   _coeffs.resize(4);
//   std::string coeffstr;

//   iss.str(std::string(lambda_coeff_1.begin(), lambda_coeff_1.end()));
//   iss >> coeffstr;
//   _coeffs[0] = std::stod(coeffstr);

//   iss.str(std::string(lambda_coeff_2.begin(), lambda_coeff_2.end()));
//   iss >> coeffstr;
//   _coeffs[1] = std::stod(coeffstr);


//   iss.str(std::string(lambda_coeff_3.begin(), lambda_coeff_3.end()));
//   iss >> coeffstr;
//   _coeffs[2] = std::stod(coeffstr);


//   iss.str(std::string(lambda_coeff_4.begin(), lambda_coeff_4.end()));
//   iss >> coeffstr;
//   _coeffs[3] = std::stod(coeffstr);

//   _lg.Trace("setting default device timing...");
//   set_device_timing(100,3);


//   //find device type and setup pixel number
//   _devidx = lambda_coeff_1[31] - static_cast<int>('0');



// }

void foxtrot::devices::stellarnet::set_device_timing(unsigned short tm, unsigned char x_timing)
{
  unsigned char msb = tm >>8 ;
  unsigned char lsb = tm & 0xFF;

  unsigned char xt = 1 << (x_timing + 1) ;

  unsigned char msd = tm > 1 ? 0x1F : 0x0F;

  std::array<unsigned char,4 > data {msb,lsb,xt,msd};

  if(_dev == nullptr)
    throw foxtrot::DeviceError("invalid stored USB device");


  const unsigned char STELLARNET_SET_TIMING_bRequest = 0xB4;
  std::chrono::milliseconds rt(_timeout_ms);
  _dev->blocking_control_transfer_send(reqtp_out, STELLARNET_SET_TIMING_bRequest, 0, 0, data, rt);

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


double stellarnet::bytes_to_coeff(const std::array<unsigned char, 0x20>& bytes)
{
  std::string coeffstr;
  //lord help me
  //to be serious, I think this is needed because each address byte thing has
  //more in it than just the double of the coefficient
  std::ifstream ifs;
  ifs >> coeffstr;
  return std::stod(coeffstr);

}

double stellarnet::get_stored_coefficient(unsigned int addr)
{
  auto bytes = get_stored_bytes(addr);
  return bytes_to_coeff(bytes);
}

std::array< unsigned char, 0x20 > stellarnet::get_stored_bytes(unsigned int addr)
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
  //NOTE: explicit cast silences warning...
  std::array<unsigned char,3> out_dat {0, static_cast<unsigned char>(addr), 0};

  if(_dev == nullptr)
    throw foxtrot::DeviceError("invalid stored USB device");

  const std::uint8_t STELLARNET_READ_BYTES_OUT_bRequest = 0xB6;
  const std::uint8_t STELLARNET_READ_BYTES_IN_bRequest = 0xB5;

  auto tm = std::chrono::milliseconds(_timeout_ms);
  const unsigned EXPECTED_PKT_SIZE = 0x21;

  std::vector<unsigned char> data;
  try{
    _dev->blocking_control_transfer_send(reqtp_out, STELLARNET_READ_BYTES_OUT_bRequest,
					 0, 0, out_dat, tm);

    data = _dev->blocking_control_transfer_receive(reqtp_in, STELLARNET_READ_BYTES_IN_bRequest, 0, 0, EXPECTED_PKT_SIZE, tm);

    if(data.size() != EXPECTED_PKT_SIZE)
      {
	_lg.strm(sl::error) << "expected 0x21 bytes, got: " << std::hex << data.size() << " bytes";
	throw foxtrot::DeviceError("read bytes got less data than expected!");
      }

  }
  catch(foxtrot::protocols::LibUsbError& err)
    {
      throw foxtrot::DeviceError(err.what());
    }

  std::array<unsigned char, EXPECTED_PKT_SIZE - 1> out;
  std::copy(data.begin() +1 , data.end(), out.begin());

  return out;


}


std::vector< unsigned short> foxtrot::devices::stellarnet::read_spectrum(int int_time)
{

  set_device_timing(int_time,3);

  int timeout = (int_time + 500) ;
  _lg.Trace("beginning spectrum integration");

  char data[1] {0};

  if(_dev == nullptr)
    throw foxtrot::DeviceError("invalid stored USB device");

  const unsigned char STELLARNET_BEGIN_SPECTRUM_bRequest = 0xB2;
  std::chrono::milliseconds rt(_timeout_ms);
  std::span<unsigned char> empty;
  _dev->blocking_control_transfer_send(reqtp_out, STELLARNET_BEGIN_SPECTRUM_bRequest,
				       0, 0, empty, rt); 
  
  
  _lg.Trace("waiting for integration...");

  //OK, FOR NOW assume int_time is in ms... CHECK if this timing works out!!!
  std::chrono::milliseconds int_time_ms(int_time);
  std::this_thread::sleep_for(int_time_ms);

  int count_ms = 0;
  while(true)
  {
    _lg.Trace("checking if data ready...");
    const unsigned char STELLARNET_CHECK_DATA_READY_bReqest = 0xB3;
    auto response = _dev->blocking_control_transfer_receive(reqtp_in, STELLARNET_CHECK_DATA_READY_bReqest,
					    0, 0, 2, rt);

    if(response.size() != 2)
      throw foxtrot::DeviceError("read back wrong amount of data!");
    
    if(response[1])
    {
      //apparently this means the data is ready...
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
  int npix = pixel_map.at(_devidx);

  //NOTE: This likely corresponds to some usual type of request
  const unsigned char STELLARNET_READBACK_ENDPOINT = 0x88;

  auto rawdata = _dev->blocking_bulk_transfer_receive(STELLARNET_READBACK_ENDPOINT,
						      npix*2, rt);
  
  
  if(rawdata.size() != npix * 2)
    throw DeviceError("failed to read back enough spectrum data!");

  std::vector<unsigned short> specdata(reinterpret_cast<unsigned short*>(rawdata.data()),
				       reinterpret_cast<unsigned short*>(rawdata.data()+rawdata.size()));
  

  return specdata;

}


std::array< double, 4> foxtrot::devices::stellarnet::get_coeffs() const
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
