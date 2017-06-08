#pragma once
#include <QWidget>
#include <QTimer>
#include "ui_dashboard_main.h"

namespace foxtrot
{
    class Client;
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
    foxtrot::Client * _client;
    int _heater_devid = -1;
    int _presgauge_devid = -1;
    int _archon_devid = -1;
    
};
