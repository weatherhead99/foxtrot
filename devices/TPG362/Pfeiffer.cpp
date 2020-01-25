#include "Pfeiffer.h"

#include <numeric>
#include <thread>
#include <cmath>

#include <foxtrot/DeviceError.h>
#include <foxtrot/protocols/SerialPort.h>

using namespace foxtrot;
using namespace foxtrot::devices;

PfeifferDevice::PfeifferDevice(shared_ptr<foxtrot::SerialProtocol> proto,
    const string& logname)
: CmdDevice(proto), _serproto(proto), _lg(logname) 
{
    
}

string PfeifferDevice::cmd(const string& request)
{
    //TODO: is this thread safe??
    _cmdoss.str("");
    _cmdoss << request << calculate_checksum(request) << '\r';
    _serproto->write(_cmdoss.str());
    
      //HACK: need a better way to wait for the reply
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto repl = _serproto->read_until_endl('\r');
    return repl;
    
}

string PfeifferDevice::calculate_checksum(const string_view message)
{
    auto checksum = std::accumulate(message.cbegin(), message.cend(), 0,
                                     [] (const char& c1, const char& c2)
                                     {
                                         return static_cast<int>(c1) + static_cast<int>(c2);
                                     });
    _lg.strm(sl::trace) << "length of message: " << message.size() ;
    _lg.strm(sl::trace) << "numeric calculated checksum: " << checksum;
    
    return str_from_number(checksum % 256, 3);
};

string PfeifferDevice::semantic_command(unsigned short address, unsigned short parameter,
                                    pfeiffer_action readwrite, optional<string_view> data)
{
    _semanticoss.str("");
    _semanticoss << str_from_number(address,3) <<
                    str_from_number(static_cast<short unsigned>(readwrite),2) <<
                    str_from_number(parameter,3);
                    
    switch(readwrite)
    {
        case pfeiffer_action::read:
            _semanticoss << "02=?";
            break;
        case pfeiffer_action::describe:
            if(!data)
            {
                const std::string errmess = "must have data present for write action";
                _lg.Error(errmess);
                throw std::logic_error(errmess);
            }
            _semanticoss << str_from_number(data->size(),2) << *data;
    }
    
    return cmd(_semanticoss.str());
}

void PfeifferDevice::validate_response_telegram_parameters(unsigned short address, 
                                                           unsigned short parameter, 
                                                           const std::tuple<int, int, string>& resp)
{
    if(std::get<0>(resp) != address)
        throw DeviceError("bad device address in reply!");
    
    if(std::get<1>(resp) != static_cast<short unsigned>(parameter))
        throw DeviceError("mismatched parameter number in reply!");
    
    
    if(std::get<2>(resp).size() == 6)
    {
        if(std::get<2>(resp).compare(0,6,"NO_DEF") == 0)
            throw DeviceError("the parameter number does not exist");
        else if(std::get<2>(resp).compare(0,6,"_RANGE") == 0)
            throw DeviceError("the parameter is out of range");
        else if(std::get<2>(resp).compare(0,6,"_LOGIC") == 0)
            throw DeviceError("Logical Access Violation");
    }
        
    
}

std::tuple<int, int, string> PfeifferDevice::interpret_response_telegram(const string& response)
{
  auto csum_calc = calculate_checksum(response.substr(0,response.size()-4));
    auto csumstr = response.substr(response.size() -4, 3);
    
    if(response.compare(response.size()-4,3,csum_calc) != 0)
    {
        _lg.strm(sl::error) << "mismatched checksum error";
        _lg.strm(sl::error) << "calculated checksum is: " << csum_calc;
        _lg.strm(sl::error) << "received checksum: " << csumstr;
	_lg.strm(sl::error) << "full response: " << response;
        throw DeviceError("got invalid checksum");
    }
    
    auto addr = std::stoi(response.substr(0,3));
    auto paramno = std::stoi(response.substr(5,3));
    auto dlen = std::stoi(response.substr(8,2));
    
    auto data = response.substr(10,dlen);
    
    return std::make_tuple(addr,paramno,data);
    
}


double foxtrot::devices::pfeiffer_interpret_u_expo_raw(const string& val)
{
    double mantissa = std::stoi(val.substr(0,4))/ 1000.;
    short exponent = std::stoi(val.substr(4,2)) - 20;
    return mantissa * std::pow(10,exponent);
}


