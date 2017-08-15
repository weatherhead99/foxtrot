#include <QApplication>
#include "dashboard_main.h"
#include <backward.hpp>
#include <iostream>

int main(int argc, char** argv)
{
//      backward::SignalHandling sh;
    QApplication app(argc,argv);
    
    Dashboard dash;
    
    
    
    dash.show();
    
    dash.setWindowState(Qt::WindowFullScreen);
    
    
    return app.exec();
    
}
