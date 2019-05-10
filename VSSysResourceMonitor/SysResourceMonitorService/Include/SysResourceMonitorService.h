#ifndef SYSRESOURCEMONITORSERVICE_H_D0279A84_7ACB_4159_82A9_81BC381FECB7
#define SYSRESOURCEMONITORSERVICE_H_D0279A84_7ACB_4159_82A9_81BC381FECB7
/************************************************************************/
/* 服务主体                                                                     */
/************************************************************************/
#include "qtservice.h"
#include <QTimer>
#include <QObject>
#include <windows.h>

class SysResourceMonitorService : public QObject, public QtService<QCoreApplication>
{
	Q_OBJECT

public:
	SysResourceMonitorService(int argc, char** argv);
	~SysResourceMonitorService();

private:
	virtual void start() override;
	virtual void stop() override;

private:
	void update();

private:
	QTimer m_oTimer;
	HANDLE m_hSysResourceMonitorServiceMutex;
};

#endif //SYSRESOURCEMONITORSERVICE_H_D0279A84_7ACB_4159_82A9_81BC381FECB7
