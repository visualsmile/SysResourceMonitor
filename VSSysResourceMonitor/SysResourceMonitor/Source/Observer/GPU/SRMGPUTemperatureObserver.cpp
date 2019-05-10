#include "Observer/GPU/SRMGPUTemperatureObserver.h"
#include "Observer/SRMModuleSubject.h"
#include "Observer/GPU/Nvidia/SRMNvidiaGPUInfo.h"

REG_SRM_OBSERVER(SRMGPUTemperatureObserver);
SRMGPUTemperatureObserver::SRMGPUTemperatureObserver()
	: SRMObserverBase("SRMGPUTemperatureObserver")
	, m_enGPUType(egtNune)
{

}

SRMGPUTemperatureObserver::~SRMGPUTemperatureObserver()
{

}

bool SRMGPUTemperatureObserver::init()
{
	if (SRMNvidiaGPUInfo::getInstance()->isValid())
	{
		m_enGPUType = egtNvidia;
		return true;
	}

	return false;
}

void SRMGPUTemperatureObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurGPUTemperature = getGPUTemperature();

	static VSShareMemTextNode oValueNode(0, 0, 0, L"0¡æ");
	QColor oCurColor = fillCollorByValue(nCurGPUTemperature, &oValueNode);
	oValueNode.m_sInfo = QString::fromStdWString(L"%1¡æ").arg(nCurGPUTemperature, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"GPU");
	fillCollorByValue(nCurGPUTemperature, &oNameNode);
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString::fromStdWString(L"GPU:%1¡æ").arg(nCurGPUTemperature, 2, 10, QChar(' ')));
}

void SRMGPUTemperatureObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"GPUÎÂ¶È"));
}

int SRMGPUTemperatureObserver::order()
{
	return 50;
}

QString SRMGPUTemperatureObserver::customSettingDescription()
{
	return QString::fromStdWString(L"GPUÎÂ¶È:");
}

int SRMGPUTemperatureObserver::getGPUTemperature()
{
	if (m_enGPUType == egtNvidia)
	{
		NvGPUThermalSettings oInfo;
		SRMNvidiaGPUInfo::getInstance()->GetThermal(0, &oInfo);
		return oInfo.Sensor[0].CurrentTemp;
	}

	return 0;
}
