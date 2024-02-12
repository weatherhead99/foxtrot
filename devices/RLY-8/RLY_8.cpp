#include "RLY_8.hh"

#include <rttr/registration>
#include <sstream>
#include <stdexcept>
#include <bitset>
#include <foxtrot/DeviceError.h>
#include <system_error>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/ProtocolError.h>



using namespace foxtrot::devices;
using namespace foxtrot;

using foxtrot::devices::RLY_8;


RLY_8::RLY_8(shared_ptr<SerialPort> proto, const std::string &comment,
             unsigned char addr)
    : CmdDevice(proto, comment), _lg("RLY_8"), _addr(addr) {

  proto->Init(nullptr);
  _lg.strm(sl::info) << "RLY_8 init done";
  //  proto->setDrain(true);
}


RLY_8::~RLY_8() {};


string RLY_8::assemble_cmd(RLY_8_Commands cmd,
                           const std::vector<unsigned char> &data) {

  _lg.strm(sl::trace) << "assemble cmd...";
  
  if(data.size() > 255)
    throw std::out_of_range("data supplied is too long!");
  

  std::vector<unsigned char> req;
  req.reserve(6 + data.size());
  
  req.push_back(0x55);
  req.push_back(0xAA);
  req.push_back(_addr);
  req.push_back(data.size());
  req.push_back(static_cast<unsigned char>(cmd));


  for(auto c : data)
    req.push_back(c);

  _lg.strm(sl::trace) << "req size: "<< req.size();
  
  auto csum = calc_checksum(req.begin(), req.end());
  req.push_back(csum);

  return std::string(req.begin(), req.end());

  
}

vector<unsigned char> RLY_8::extract_response(const string& response,
					      RLY_8_Commands expd_cmd)
{
  vector<unsigned char> out;

  std::ostringstream oss;
  for(auto c : response)
    oss << (int) c << ",";

  _lg.strm(sl::trace) << "response to extract: " << oss.str();
  

  if(0x55 != static_cast<unsigned char>(response[0]))
    throw DeviceError("first byte should be 0x55!");

  _lg.strm(sl::debug) << "second byte is: " << (unsigned char) response[1];
  
  if(0xAA != static_cast<unsigned char>(response[1]))
    throw DeviceError("second byte should be 0xAA");
  
  if(static_cast<unsigned char>(response[2]) != _addr)
    throw DeviceError("unexpected source address!");

  _lg.strm(sl::debug) << "received cmd word is: " << (int) (unsigned char) response[4];

  //don't quite understand this yet...
  // if(static_cast<unsigned char>(response[4]) != static_cast<unsigned char>(expd_cmd))
  //   throw DeviceError("unexpected cmd word!");


  unsigned char recv_csum = *(response.rbegin());
  unsigned char calc_csum = calc_checksum(response.begin(), response.begin() + response.size() -1);

  if(recv_csum != calc_csum)
    {
      std::ostringstream oss;
      oss << "received checksum was: " << (int) recv_csum << ", but expected: " << (int) calc_csum;
      throw DeviceError(oss.str());
    }

  auto datbegin = response.cbegin() + 5;
  auto datend = datbegin + response[3];

  out.resize(response[3]);
  std::copy(datbegin, datend, out.begin());
  
  return out;
}

string RLY_8::cmd(const string& request)
{
  auto protoptr = std::static_pointer_cast<foxtrot::protocols::SerialPort>(_proto);
  
  _lg.strm(sl::trace) << "request is: " << request;
  protoptr->write(request);

  auto timeout = protoptr->calc_minimum_transfer_time(256) * 100;

  //will at least return the same amount of the request
  auto ret = protoptr->read_all(6, timeout);

  _lg.strm(sl::trace) << "return length: " << ret.size();
  return ret;
}

string RLY_8::getVersion()
{
  std::vector<unsigned char> data;
  auto thisreq = assemble_cmd(RLY_8_Commands::QUERY_VERSION, data);


  std::ostringstream oss;
  oss << std::hex;
  for(auto& c  : thisreq)
    oss << (int) c << ",";
  
  _lg.strm(sl::debug) << "thisreq: " <<  oss.str();
  
  auto ret = cmd(thisreq);

  oss.str("");

  for(auto& c : ret)
    oss << c;
  
  _lg.strm(sl::debug) << "return: " << oss.str();

  auto outdat = extract_response(ret, RLY_8_Commands::QUERY_VERSION);
  return std::string(outdat.begin(), outdat.end());
}


unsigned char RLY_8::getRelayState()
{
  std::vector<unsigned char> data;
  auto thisreq = assemble_cmd(RLY_8_Commands::QUERY_RELAY_STATE, data);
  auto ret = cmd(thisreq);
  auto outdat = extract_response(ret, RLY_8_Commands::QUERY_RELAY_STATE);

  if(outdat.size() != 1)
    throw std::out_of_range("expected data of length 1 return");
  
  return outdat[0];
}

void RLY_8::setRelay(unsigned char channel, bool onoff)
{

  if(channel < 1 or channel > 8)
    throw std::out_of_range("invalid relay number, must be between 1 and 8");
  
  std::vector<unsigned char> data;
  data.resize(2);

  data[0] = channel;
  data[1] = onoff;

  auto thisreq = assemble_cmd(RLY_8_Commands::SET_RELAY, data);

  std::ostringstream oss;
  oss << std::hex;
  for(auto& c  : thisreq)
    oss << (int) c << ",";
  
  _lg.strm(sl::debug) << "thisreq: " <<  oss.str();
  



  auto ret = cmd(thisreq);

  auto outdat = extract_response(ret, RLY_8_Commands::SET_RELAY);

  if(outdat.size() != 1)
    throw std::out_of_range("expected data of length 1 return!");

  if(outdat[0] != 0)
    throw foxtrot::DeviceError("failed to set relay for some reason!");
}

bool RLY_8::getRelay(unsigned char channel)
{
  auto state = getRelayState();
  auto bs = std::bitset<8>(state);
  return bs[channel-1];
}


RTTR_REGISTRATION{

  using namespace rttr;
  using foxtrot::devices::RLY_8;

  registration::class_<RLY_8>("foxtrot::devices::RLY_8")
    .method("getRelay", &RLY_8::getRelay)(parameter_names("channel"))
    .method("setRelay", &RLY_8::setRelay)(parameter_names("channel", "onoff"))
    .property_readonly("RelayState", &RLY_8::getRelayState)
    .property_readonly("Version", &RLY_8::getVersion);

}
