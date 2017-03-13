#include "dashboard_main.h"
#include "client.h"
#include <future>
#include <thread>
#include <chrono>


Dashboard::Dashboard(QWidget* parent)
: QMainWindow(parent)
{
    ui.setupUi(this);
    
    ui.statusbar->showMessage("starting up");
    
    QObject::connect(ui.actionConnect, &QAction::triggered, this, &Dashboard::connectServer);
    
}

Dashboard::~Dashboard()
{
 if(_client)
 {
     delete _client;
 }
    
}

void Dashboard::connectServer()
{
    auto async_connect_client = [this] () {
        ui.statusbar->showMessage("connecting...");
      _client = new foxtrot::Client("localhost:50051");
      ui.statusbar->showMessage("connected to server");
      
      
        
    };
    
    std::async(async_connect_client);
    
}


