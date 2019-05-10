#ifndef SYSRESOURCEMONITOR_H_2172739D_7EF2_4283_8B3E_D5177C45A37F
#define SYSRESOURCEMONITOR_H_2172739D_7EF2_4283_8B3E_D5177C45A37F
/************************************************************************/
/* 主窗口，主流程                                                                     */
/************************************************************************/
#include <QMainWindow>
#include <QList>
#include <QLabel>
#include <QTimer>

class SRMFloatingWidget;
class VSSharedMemStruct;
class SRMSystemTray;

class SysResourceMonitor : public QMainWindow
{
	Q_OBJECT
public:
	SysResourceMonitor(QWidget* parent = Q_NULLPTR);
	~SysResourceMonitor();
	void doWork();

private:
	void update();
	void updateWidget();
	void updateDeskBand();	

private:
	SRMFloatingWidget *m_pFloatingWidget;
	QList<QLabel*> m_oLabelList;
	SRMSystemTray* m_pSystemTray;

private:
	VSSharedMemStruct* m_pVSSharedMemStruct;
	QTimer m_oTimer;
};
#endif //SYSRESOURCEMONITOR_H_2172739D_7EF2_4283_8B3E_D5177C45A37F