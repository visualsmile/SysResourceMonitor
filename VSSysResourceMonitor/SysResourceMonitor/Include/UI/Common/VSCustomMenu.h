#ifndef SRMCUSTOMMENU_H_CE008758_029F_4834_A605_C3430BD404DC
#define SRMCUSTOMMENU_H_CE008758_029F_4834_A605_C3430BD404DC
/************************************************************************/
/*重写QMenu
  以解决Qt中托盘菜单不能自动消失的问题                                                                     */
/************************************************************************/
#include <QMenu>
#include <windows.h>

class VSCustomMenu : public QMenu
{
public:
	VSCustomMenu(QWidget* p) :QMenu(p) 
	{

	}
	~VSCustomMenu() 
	{

	}

private:
	virtual void paintEvent(QPaintEvent* e) override 
	{
		SetForegroundWindow(HWND(this->winId()));
		QMenu::paintEvent(e);
	}
};

#endif //SRMCUSTOMMENU_H_CE008758_029F_4834_A605_C3430BD404DC
