#include "dashboard_main.h"
#include "client.h"
#include <future>
#include <thread>
#include <chrono>
#include <exception>
#include <QDateTime>


Dashboard::Dashboard(QWidget* parent)
: QMainWindow(parent), _timer(this)
{
 
 
  
    ui.setupUi(this);
    
    ui.statusbar->showMessage("starting up");
    
    this->setWindowState(Qt::WindowMaximized);
    
    QObject::connect(ui.actionConnect, &QAction::triggered, this, &Dashboard::connectServer);
    QObject::connect(ui.actionManual_update, &QAction::triggered, this, &Dashboard::updateTempReadings);
    QObject::connect(ui.actionAuto_update, &QAction::toggled, this, &Dashboard::setautoupdate);
    QObject::connect(&_timer,&QTimer::timeout, this, &Dashboard::updateTempReadings);
    
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
      
      _heater_devid = foxtrot::find_devid_on_server(servdesc,"ArchonHeaterX");
      if(_heater_devid < 0)
      {
	ui.statusbar->showMessage("no heater module found");
      }
      _presgauge_devid = foxtrot::find_devid_on_server(servdesc, "TPG362");
      if(_presgauge_devid < 0)
      {
	ui.statusbar->showMessage("no pressure gauge found");
      }
      
      _archon_devid = foxtrot::find_devid_on_server(servdesc,"archon");
      if(_archon_devid < 0)
      {
	ui.statusbar->showMessage("no archon found");
      }
      
      
      
        }
        catch(...)
        {
            ui.statusbar->showMessage("caught exception...");
	    rethrow_error(std::current_exception());   
        }
      
      
    };
    
    auto result = std::async(std::launch::async,async_connect_client);
    
    
}

void Dashboard::updateTempReadings()
{
  if(_heater_devid < 0 || _presgauge_devid < 0 || _archon_devid < 0)
  {
   ui.statusbar->showMessage("no devices setup, cannot update");
   return;
  }
  
  if(!_client)
  {
    ui.statusbar->showMessage("no client setup, cannot update");
    return; 
  }
  
  _client->InvokeCapability(_archon_devid,"update_state");
  auto tank_temp = boost::get<double>(_client->InvokeCapability(_heater_devid,"getTempA"));
  auto stage_temp = boost::get<double>(_client->InvokeCapability(_heater_devid,"getTempB"));

  ui.tank_temp_display->display(tank_temp);
  ui.stage_temp_display->display(stage_temp);
  
  
  std::vector<foxtrot::ft_variant> args{0};
  auto heater_target = boost::get<double>(_client->InvokeCapability(_heater_devid,"getHeaterTarget",args.begin(),args.end()));
  
  auto heater_output = boost::get<double>(_client->InvokeCapability(_heater_devid,"getHeaterAOutput"));
  
  ui.heater_output->display(heater_output / 25. * 100);
  ui.heater_target->display(heater_target);
  
  
  auto now = QDateTime::currentDateTime();
  
  ui.statusbar->showMessage("last update: " + now.toString());
  
}

void Dashboard::setautoupdate(bool onoff)
{
  if(onoff)
  {
    ui.statusbar->showMessage("auto update enabling");
    _timer.start(ui.updatetimeout->value());
    
  }
  else
  {
    ui.statusbar->showMessage("auto update disabling");
    _timer.stop();
  }
  
  
}



void Dashboard::rethrow_error(std::exception_ptr pt)
{
    std::rethrow_exception(pt);
    
    
}
