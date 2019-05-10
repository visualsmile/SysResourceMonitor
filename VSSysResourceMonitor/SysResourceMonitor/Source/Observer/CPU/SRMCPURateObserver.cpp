#include "Observer/CPU/SRMCPURateObserver.h"
#include "VSDeskBandOperation.h"
#include "SRMConfig.h"
#include "Observer/SRMModuleSubject.h"
#include <QAction>
#include <QMenu>

REG_SRM_OBSERVER(SRMCPURateObserver);
SRMCPURateObserver::SRMCPURateObserver()
	: SRMObserverBase("SRMCPURateObserver")
{

}

SRMCPURateObserver::~SRMCPURateObserver()
{

}

bool SRMCPURateObserver::init()
{
	return true;
}

void SRMCPURateObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurCPURate = getCPURate();	

	static VSShareMemTextNode oValueNode(0, 0, 0, L"0%");
	QColor oCurColor = fillCollorByValue(nCurCPURate, &oValueNode);
	oValueNode.m_sInfo = QString("%1%").arg(nCurCPURate, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"CPU");
	fillCollorByValue(nCurCPURate, &oNameNode);
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString("CPU:%1%").arg(nCurCPURate, 2, 10, QChar(' ')));
}

void SRMCPURateObserver::regTrayMenu(QMenu *pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"CPU使用"));
}

int SRMCPURateObserver::order()
{
	return 10;
}

QString SRMCPURateObserver::customSettingDescription()
{
	return QString::fromStdWString(L"CPU使用:");
}

int SRMCPURateObserver::getCPURate()
{
	FILETIME oIdleTime;
	FILETIME oKernelTime;
	FILETIME oUserTime;

	auto compareFileTime = [&](FILETIME time1, FILETIME time2) -> __int64
	{
		__int64 a = time1.dwHighDateTime << 31 | time1.dwLowDateTime;
		__int64 b = time2.dwHighDateTime << 31 | time2.dwLowDateTime;
		return   (b - a);
	};

	GetSystemTimes(&oIdleTime, &oKernelTime, &oUserTime);

	__int64 nIdle = compareFileTime(m_oPreIdleTime, oIdleTime);
	__int64 nKernel = compareFileTime(m_oPreKernelTime, oKernelTime);
	__int64 nUser = compareFileTime(m_oPreUserTime, oUserTime);

	m_oPreIdleTime = oIdleTime;
	m_oPreKernelTime = oKernelTime;
	m_oPreUserTime = oUserTime;

	if (nKernel == 0 && nUser == 0)
	{
		return 0;
	}

	int nCpuRate = (nKernel + nUser - nIdle) * 100 / (nKernel + nUser);
	if (nCpuRate <= 100 && nCpuRate >= 0)
	{
		return nCpuRate;
	}
	else
	{
		return 0;
	}
}
