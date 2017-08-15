#pragma once
#include <QWidget>
#include <QTimer>
#include "ui_dashboard_main.h"

namespace foxtrot
{
    class Client;
    class TelemetryClient;
}

class Dashboard : public QMainWindow
{
  Q_OBJECT
public:
    explicit Dashboard(QWidget* parent = nullptr);
    virtual ~Dashboard();
    
public slots:
    void connectServer();
    void updateTempReadings();
    void setautoupdate(bool onoff);
    
    
    
    
private:
    void rethrow_error(std::exception_ptr pt);
    QTimer _timer;
    Ui::MainWindow ui;
    foxtrot::TelemetryClient* _telemcl;
    
    double _tank_temp;
    double _stage_temp;
    
    double _heater_target;
    double _heater_output;
    double _heater_P;
    double _heater_I;
    double _heater_D;
    
    
    
    
};



