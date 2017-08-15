#include "dashboard_main.h"
#include "client.h"
#include <telemetry/TelemetryClient.h>
#include <future>
#include <thread>
#include <chrono>
#include <exception>
#include <QDateTime>
#include <cmath>

#include "TelemetryClient.h"


Dashboard::Dashboard(QWidget* parent)
: QMainWindow(parent), _timer(this)
{
    ui.setupUi(this);

    
    
  
#if QWT_VERSION >= 0x060103
#pragma message("using QWT new version hacks")
  ui.heater_D->setScale(-1.,1.);
  ui.heater_D->setOrientation(Qt::Horizontal);
  ui.heater_I->setScale(-1.,1.);
  ui.heater_I->setOrientation(Qt::Horizontal);
  ui.heater_P->setScale(-1.,1.);
  ui.heater_P->setOrientation(Qt::Horizontal);
  ui.heater_output->setScale(0,100.);
  ui.heater_output->setOrientation(Qt::Horizontal);
  
#endif
  
    
    ui.statusbar->showMessage("starting up");
    
//     this->setWindowState(Qt::WindowMaximized);
    
    QObject::connect(ui.actionConnect, &QAction::triggered, this, &Dashboard::connectServer);
    QObject::connect(ui.actionManual_update, &QAction::triggered, this, &Dashboard::updateTempReadings);
    QObject::connect(ui.actionAuto_update, &QAction::toggled, this, &Dashboard::setautoupdate);
    QObject::connect(&_timer,&QTimer::timeout, this, &Dashboard::updateTempReadings);
    
    connectServer();
    setautoupdate(true);
    
}

Dashboard::~Dashboard()
{
 
 if(_telemcl)
 {
   delete _telemcl;
 }
    
}

void Dashboard::connectServer()
{
  
    _telemcl = new foxtrot::TelemetryClient;  
    _telemcl->ConnectSocket("tcp://localhost:5000");
    _telemcl->Subscribe("testbench");
  
    
    auto async_update = [this]() {
      while(true)
      {
	auto msg = _telemcl->waitMessageSync();
	
	if(msg.name == "stage_temp")
	{
	  _stage_temp = boost::get<double>(msg.value);
	  updateTempReadings();
	  continue;
	}
	if(msg.name == "tank_temp")
	{
	  _tank_temp = boost::get<double>(msg.value);
	  updateTempReadings();
	  continue;
	}
	if(msg.name == "heater")
	{
	  _heater_output = boost::get<double>(msg.value);
	  updateTempReadings();
	  continue;
	};
	
	if(msg.name == "heaterP")
	{
	  _heater_P = boost::get<int>(msg.value);
	  updateTempReadings();
	  continue;
	};
	
	if(msg.name == "heaterI")
	{
	  _heater_I = boost::get<int>(msg.value);
	  updateTempReadings();
	  continue;
	};
	
	if(msg.name == "heaterD")
	{
	  _heater_D = boost::get<int>(msg.value);
	  updateTempReadings();
	  continue;
	}
	
	if(msg.name == "heater_target")
	{
	  _heater_target = boost::get<int>(msg.value);
	  updateTempReadings();
	  continue;
	}
	
      };
      
      
      
    };
    
    
    std::async(std::launch::async,async_update);
    
    
}

void Dashboard::updateTempReadings()
{
  
  ui.tank_temp_display->display(_tank_temp);
  ui.stage_temp_display->display(_stage_temp);
  
  
  ui.heater_target->display(_heater_target);
    
  ui.heater_output->setValue(_heater_output / 25. * 100);
  
                                      
  auto heater_total = _heater_P + _heater_I + _heater_D;
  
  double P_pc = _heater_P / std::abs(heater_total);
  double I_pc = _heater_I / std::abs(heater_total);
  double D_pc = _heater_D / std::abs(heater_total);
  
  ui.heater_D->setValue(D_pc);
  ui.heater_I->setValue(I_pc);
  ui.heater_P->setValue(P_pc);
  
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
