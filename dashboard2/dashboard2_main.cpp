#include "dashboard2_main.h"
#include "OPMD_hlclient.h"


Dashboard2::Dashboard2(QWidget* parent)
{
  ui.setupUi(this);
  
  //setup tab switch keyboard shortcuts
  
  

}

Dashboard2::~Dashboard2()
{

}

void Dashboard2::keyPressEvent(QKeyEvent* ev)
{
  if(ev->key() == Qt::Key_z)
  {
    ui.tabWidget->setCurrentWidget(ui.cryotab);
    
  }
  else if(ev->key() == Qt::Key_x)
  {
    ui.tabWidget->setCurrentWidget(ui.CCDtab);
     
  }
  else if(ev->key() == Qt::Key_C)
  {
    ui.tabWidget->setCurrentWidget(ui.illumtab);
  }
  
 
}
