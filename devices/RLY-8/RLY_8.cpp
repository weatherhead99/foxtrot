#include "RLY_8.hh"

#include <sstream>
#include <stdexcept>
#include <foxtrot/DeviceError.h>
#include <system_error>

using namespace foxtrot::devices;
using namespace foxtrot;


RLY_8::RLY_8(shared_ptr<SerialPort> proto, const std::string &comment)
  : CmdDevice(proto, comment), _lg("RLY_8") {}


unsigned char RLY_8::calc_checksum(string::const_iterator start,
				   string::const_iterator end) const
{
  unsigned sum = 0;
  auto it = start;
  while(it != end)
    sum += *it;

  return static_cast<unsigned char>(sum % 256);
}


string RLY_8::assemble_cmd(unsigned char addr, RLY_8_Commands cmd,
                           const std::vector<unsigned char> &data) {

  if(data.size() > 255)
    throw std::out_of_range("data supplied is too long!");
  
  std::ostringstream oss;
  
  
  oss << 0x55 << 0xAA << addr << static_cast<unsigned char>(cmd) << static_cast<unsigned char>(data.size());
  for(auto c : data)
    oss << c;
  oss << calc_checksum(oss.str().cbegin(), oss.str().cend());
  return oss.str();
}

vector<unsigned char> RLY_8::extract_response(const string& response,
					      unsigned char expd_addr,
					      RLY_8_Commands expd_cmd)
{
  vector<unsigned char> out;

  if(response[0] != 0x55 or response[1] != 0xAA)
    throw DeviceError("unexpected start byte of response");
  
  if(response[2] != expd_addr)
    throw DeviceError("unexpected source address!");

  if(response[4] != static_cast<unsigned char>(expd_cmd))
    throw DeviceError("unexpected cmd word!");


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
  _lg.strm(sl::trace) << "request is: " << request;
  _proto->write(request);

  auto timeout = _proto->calc_minimum_transfer_time(request.size()) * 10;
  unsigned actlen;

  
}

