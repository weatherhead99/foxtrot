#include <iostream>
#include "Logging.h"
#include <termcolor.hpp>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    
    foxtrot::Logging lg("log dev");
    
    foxtrot::setDefaultSink();
    
    lg.Fatal("fatal");
    lg.Error("error");
    lg.Warning("warn");
    lg.Info("info");
    lg.Debug("debug");
    lg.Trace("Trace");

    


}
