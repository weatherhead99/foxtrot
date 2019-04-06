#pragma once
#include "ui_dashboard2.h"

class OPMD_hlclient;

class Dashboard2 : public QMainWindow
{
  Q_OBJECT
public:
  explicit Dashboard2(QWidget* parent = nullptr);
  virtual ~Dashboard2();
  
  virtual void keyPressEvent(QKeyEvent* ev);
  
private:
  OPMD_hlclient* _client;
  Ui::MainWindow ui;
  
};