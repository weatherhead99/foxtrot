#pragma once


#include <memory>
#include <string>
#include <optional>
#include <iomanip>
#include <string_view>

#include <foxtrot/Logging.h>
#include <foxtrot/server/CmdDevice.h>
#include <foxtrot/protocols/SerialProtocol.h>

using std::shared_ptr;
using std::string;
using std::optional;
using std::string_view;

namespace foxtrot
{
namespace devices
{
    enum class pfeiffer_action : short unsigned
    {
        read = 0,
        describe = 10
    };
    
    class PfeifferDevice : public CmdDevice
    {
    RTTR_ENABLE()
    public:
        virtual string cmd(const string& request) override;
    protected:
        PfeifferDevice(shared_ptr<SerialProtocol> proto, const string& logname);
        string semantic_cmd(short unsigned address, short unsigned parameter,
                            pfeiffer_action readwrite, optional<string_view> data = std::nullopt);
        
        template<typename T>
        string semantic_command(short unsigned address, T parameter,
                                pfeiffer_action readwrite, optional<string_view> data = std::nullopt)
        {
            return semantic_cmd(address, static_cast<short unsigned>(parameter), readwrite, data);
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
        string str_from_number(const T& number, unsigned short width=3)
        {
            std::ostringstream oss;
            oss << std::setw(width) << std::setfill('0') << number;
            return oss.str();
        };
    private:
        string calculate_checksum(string::const_iterator start, string::const_iterator end);
        string calculate_checksum(const string_view message);
        
        std::ostringstream _cmdoss;
        std::ostringstream _semanticoss;
        
    };
    
}
}
