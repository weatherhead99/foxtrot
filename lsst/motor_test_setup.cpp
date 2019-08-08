#include <foxtrot/server/DeviceHarness.h>
#include <foxtrot/protocols/SerialPort.h>
#include <map>
#include <foxtrot/devices/BSC203.h>
#include <foxtrot/devices/TIM101.h>
#include <foxtrot/devices/idscamera.h>


using mapofparametersets = std::map<std::string, foxtrot::parameterset>;

//Motor intialization (Serial ports)
foxtrot::parameterset sport_params_tim {
    {"port", "/dev/ttyUSB11"},
    {"flowcontrol", "hardware"}
};

foxtrot::parameterset sport_params_bsc {
    {"port", "/dev/ttyUSB1"},
    {"flowcontrol", "hardware"}
};

//Camera initialization (ID)
const uint32_t cameraID = 4;

extern "C" {

    int setup ( foxtrot::DeviceHarness& harness, const mapofparametersets* const params )
    {
        foxtrot::Logging lg ( "setup" );

//        if ( params == nullptr ) {
//            lg.Fatal ( "no parametersets received, cannot continue.." );
//            throw std::runtime_error ( "setup cannot continue" );
//        }

        lg.Info ( "setting up TIM101" );
        auto sport_tim = std::make_shared<foxtrot::protocols::SerialPort> ( &sport_params_tim );
        auto presgauge_tim = std::unique_ptr<foxtrot::devices::TIM101> ( new foxtrot::devices::TIM101 ( sport_tim ) );
        harness.AddDevice ( std::move ( presgauge_tim ) );


        lg.Info ( "setting up BSC203" );
        auto sport_bsc = std::make_shared<foxtrot::protocols::SerialPort> ( &sport_params_bsc );
        auto presgauge_bsc = std::unique_ptr<foxtrot::devices::BSC203> ( new foxtrot::devices::BSC203 ( sport_bsc ) );
        harness.AddDevice ( std::move ( presgauge_bsc ) );
        
        lg.Info("setting up Camera");
        auto presgauge_cam = std::unique_ptr<foxtrot::devices::idscamera> ( new foxtrot::devices::idscamera ( &cameraID ) );
        harness.AddDevice ( std::move ( presgauge_cam ) );

        return 0;

    }
}
