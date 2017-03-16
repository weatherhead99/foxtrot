#include "ServerUtil.h"
#include "Logging.h"


rttr::variant foxtrot::get_arg(const capability_argument& arg, bool& success)
    {
    success=true;
    rttr::variant outarg;
    auto which_type = arg.value_case();
    
    foxtrot::Logging lg("get_arg");
    lg.Debug("arg type switch: " + std::to_string(which_type) );
    
    switch(which_type)
    {
    case(capability_argument::ValueCase::kDblarg):
        outarg = arg.dblarg();
        break;
        
    case(capability_argument::ValueCase::kIntarg):
        outarg = arg.intarg();
        break;
        
    case(capability_argument::ValueCase::kBoolarg):
        
        outarg = arg.boolarg();
        break;
    
    case(capability_argument::ValueCase::kStrarg):
        
        outarg = arg.strarg();
        break;

    case(capability_argument::ValueCase::VALUE_NOT_SET):
        success = false;
    }
  
    //     cout << "outarg type: " << outarg.get_type().get_name() << endl;
    return outarg;
    }
    
    