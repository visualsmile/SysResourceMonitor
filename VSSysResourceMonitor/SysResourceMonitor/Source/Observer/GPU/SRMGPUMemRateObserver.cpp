
#include "Observer/GPU/SRMGPUMemRateObserver.h"
#include "Observer/SRMModuleSubject.h"
#include "Observer/GPU/Nvidia/SRMNvidiaGPUInfo.h"

REG_SRM_OBSERVER(SRMGPUMemRateObserver);
SRMGPUMemRateObserver::SRMGPUMemRateObserver()
	: SRMObserverBase("SRMGPUMemRateObserver")
	, m_enGPUType(egtNune)
{

}

SRMGPUMemRateObserver::~SRMGPUMemRateObserver()
{

}

bool SRMGPUMemRateObserver::init()
{
	if (SRMNvidiaGPUInfo::getInstance()->isValid())
	{
		m_enGPUType = egtNvidia;
		return true;
	}

	return false;
}

void SRMGPUMemRateObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurGPUMemRate = getGPUMamRate();

	static VSShareMemTextNode oValueNode(0, 0, 0, L"0%");
	QColor oCurColor = fillCollorByValue(nCurGPUMemRate, &oValueNode);
	oValueNode.m_sInfo = QString("%1%").arg(nCurGPUMemRate, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"DRAM");
	fillCollorByValue(nCurGPUMemRate, &oNameNode);
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString("DRAM:%1%").arg(nCurGPUMemRate, 2, 10, QChar(' ')));
}

void SRMGPUMemRateObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"显存使用"));
}

int SRMGPUMemRateObserver::order()
{
	return 60;
}

QString SRMGPUMemRateObserver::customSettingDescription()
{
	return QString::fromStdWString(L"显存使用:");
}

int SRMGPUMemRateObserver::getGPUMamRate()
{
	if (m_enGPUType == egtNvidia)
	{
		DISPLAY_INFO oInfo;
		SRMNvidiaGPUInfo::getInstance()->GetDisplayInfo(0, &oInfo);
		return 1.0* (oInfo.dwTotalMemory - oInfo.dwFreeMemory) / oInfo.dwTotalMemory * 100;
	}

	return 0;
}

