#pragma once


#include <memory>
#include <string>
#include <optional>
#include <iomanip>
#include <string_view>

#include <foxtrot/Logging.h>
#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/SerialProtocol.h>

using std::shared_ptr;
using std::string;
using std::optional;


namespace foxtrot
{
namespace devices
{
    
  using string_view = std::basic_string_view<char>;
    enum class pfeiffer_action : short unsigned
    {
        read = 0,
        describe = 10
    };
    
    
    enum class pfeiffer_data_types : short unsigned
    {
        boolean = 0,
        positive_integer_long = 1,
        positive_fixed_comma_number = 2,
        symbol_chain_short  = 4,
	  new_boolean = 6,
        positive_integer_short = 7,
        symbol_chain_long = 11
    };
    
    class PfeifferDevice : public CmdDevice
    {
    RTTR_ENABLE()
    public:
        virtual string cmd(const string& request) override;
    protected:
        PfeifferDevice(shared_ptr<SerialProtocol> proto, const string& logname);
        string semantic_command(short unsigned address, short unsigned parameter,
                            pfeiffer_action readwrite, optional<string_view> data = std::nullopt);
        
        template<typename T>
        string semantic_command(short unsigned address, T parameter,
                                pfeiffer_action readwrite, optional<string_view> data = std::nullopt)
        {
            return semantic_command(address, static_cast<short unsigned>(parameter), readwrite, data);
        }
        
        shared_ptr<SerialProtocol> _serproto;
        foxtrot::Logging _lg;
        
        std::tuple<int,int,string> interpret_response_telegram(const string& response);
        
        template <typename T> 
        void validate_response_telegram_parameters(unsigned short address, T parameter, 
                                                   const std::tuple<int,int,string>& resp)
        {
            validate_response_telegram_parameters(address, static_cast<short unsigned>(parameter), resp);
        }
        
        void validate_response_telegram_parameters(unsigned short address, unsigned short parameter, const std::tuple<int,int,string>& resp);
        
        template<typename T> 
        string str_from_number(const T& number, unsigned short width)
        {
            std::ostringstream oss;
            oss << std::setw(width) << std::setfill('0') << number;
            return oss.str();
        };
        
        template<typename T>
        string str_from_number(const T& number, pfeiffer_data_types dtype)
        {
            
            using foxtrot::devices::pfeiffer_data_types;
            
            std::ostringstream oss;
            switch(dtype)
            {
                case(pfeiffer_data_types::boolean):
                    oss << std::setw(1);
                    for(int i=0; i< 6; i++)
                        oss << number;
                    break;
	    case(pfeiffer_data_types::new_boolean):
	      oss << std::setw(1);
	      oss << number;
	      
	      break;
                case(pfeiffer_data_types::positive_integer_long):
                case(pfeiffer_data_types::positive_fixed_comma_number):
                case(pfeiffer_data_types::symbol_chain_short):
                    oss << std::setw(6) << std::setfill('0') << number;
                    break;
                case(pfeiffer_data_types::positive_integer_short):
                    oss << std::setw(3) <<  std::setfill('0') << number;
                    break;
                case(pfeiffer_data_types::symbol_chain_long):
                    oss << std::setw(16) << std::setfill(' ') << number;
                default:
                    throw std::logic_error("invalid pfieffer data type!");
                    
            }
            return oss.str();
        }
        
        
        
        
        template<typename T>
        string read_cmd_helper(int addr, T param)
        {
            auto ret = semantic_command(addr, param, pfeiffer_action::read);
            auto interpret = interpret_response_telegram(ret);
            
            validate_response_telegram_parameters(addr, param, interpret);

            return std::get<2>(interpret);
        };
        
        template<typename T1, typename T2>
        void write_cmd_helper(int addr, T1 param, T2 value, pfeiffer_data_types dtype)
        {
            auto st = str_from_number(value, dtype);
            auto ret = semantic_command(addr, param, pfeiffer_action::describe, st);
            
            auto interpret = interpret_response_telegram(ret);
            validate_response_telegram_parameters(addr, param, interpret);
            
        };
        
        
    private:
        string calculate_checksum(string::const_iterator start, string::const_iterator end);
        string calculate_checksum(const string_view message);
        
        std::ostringstream _cmdoss;
        std::ostringstream _semanticoss;
      bool _is_serial_port;
        
    };
    
    
    double pfeiffer_interpret_u_expo_raw(const string& val);
    
}
}
