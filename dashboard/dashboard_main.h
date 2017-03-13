#pragma once
#include <QWidget>
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
//     void updateTempReadings();
  
    
    
private:
    Ui::MainWindow ui;
    foxtrot::Client * _client;
};
