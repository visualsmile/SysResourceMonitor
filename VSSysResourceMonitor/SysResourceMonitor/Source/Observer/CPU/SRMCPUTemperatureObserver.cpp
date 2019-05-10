#include "Observer/CPU/SRMCPUTemperatureObserver.h"
#include "Observer/CPU/SRMCPUTemperatureServiceObserver.h"
#include "Observer/SRMModuleSubject.h"

REG_SRM_OBSERVER(SRMCPUTemperatureObserver);
SRMCPUTemperatureObserver::SRMCPUTemperatureObserver()
	: SRMServiceReadObserverBase(s_sCPUTemperatureServiceWriteMutexSemaphoreName, s_sCPUTemperatureServiceShareMemoryName, "SRMCPUTemperatureObserver")
{

}

SRMCPUTemperatureObserver::~SRMCPUTemperatureObserver()
{

}

bool SRMCPUTemperatureObserver::init()
{
	return __super::init();
}

void SRMCPUTemperatureObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurCPUTemperature = readServiceData();

	static VSShareMemTextNode oValueNode(0, 0, 0, L"0¡æ");
	QColor oCurColor = fillCollorByValue(nCurCPUTemperature, &oValueNode);
	oValueNode.m_sInfo = QString::fromStdWString(L"%1¡æ").arg(nCurCPUTemperature, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"CPU");
	fillCollorByValue(nCurCPUTemperature, &oNameNode);
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString::fromStdWString(L"CPU:%1¡æ").arg(nCurCPUTemperature, 2, 10, QChar(' ')));
}

void SRMCPUTemperatureObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"CPUÎÂ¶È"));
}

int SRMCPUTemperatureObserver::order()
{
	return 20;
}

QString SRMCPUTemperatureObserver::customSettingDescription()
{
	return QString::fromStdWString(L"CPUÎÂ¶È:");
}