#include <foxtrot/server/DeviceHarness.h>
#include <foxtrot/protocols/SerialPort.h>
#include <map>
#include <foxtrot/devices/BSC203.h>
#include <foxtrot/devices/TIM101.h>


using mapofparametersets = std::map<std::string, foxtrot::parameterset>;

foxtrot::parameterset sport_params_tim {
    {"port", "/dev/ttyUSB0"},
    {"flowcontrol", "hardware"}
};

foxtrot::parameterset sport_params_bsc {
    {"port", "/dev/ttyUSB1"},
    {"flowcontrol", "hardware"}
};



extern "C" {

    int setup ( foxtrot::DeviceHarness& harness, const mapofparametersets* const params )
    {
        foxtrot::Logging lg ( "setup" );

        if ( params == nullptr ) {
            lg.Fatal ( "no parametersets received, cannot continue.." );
            throw std::runtime_error ( "setup cannot continue" );
        }

        lg.Info ( "setting up TIM101" );
        auto sport_tim = std::make_shared<foxtrot::protocols::SerialPort> ( &sport_params_tim );
        auto presgauge_tim = std::unique_ptr<foxtrot::devices::TIM101> ( new foxtrot::devices::TIM101 ( sport_tim ) );
        harness.AddDevice ( std::move ( presgauge_tim ) );


        lg.Info ( "setting up BSC203" );
        auto sport_bsc = std::make_shared<foxtrot::protocols::SerialPort> ( &sport_params_bsc );
        auto presgauge_bsc = std::unique_ptr<foxtrot::devices::BSC203> ( new foxtrot::devices::BSC203 ( sport_bsc ) );
        harness.AddDevice ( std::move ( presgauge_bsc ) );

        return 0;

    }
}
