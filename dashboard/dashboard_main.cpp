#include "dashboard_main.h"
#include "client.h"
#include <future>
#include <thread>
#include <chrono>
#include <exception>

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
        
        try{
        ui.statusbar->showMessage("connecting...");      
      _client = new foxtrot::Client("localhost:50051");
      ui.statusbar->showMessage("connected to server"); 

      ui.statusbar->showMessage("finding archon heater device...");
      
      auto servdesc = _client->DescribeServer();
      
        }
        catch(...)
        {
            ui.statusbar->showMessage("caught exception...");
            
         rethrow_error(std::current_exception());   
            
        }
      
      
    };
    
    auto result = std::async(std::launch::async,async_connect_client);
    
    
}


void Dashboard::rethrow_error(std::exception_ptr pt)
{
    std::rethrow_exception(pt);
    
    
}
