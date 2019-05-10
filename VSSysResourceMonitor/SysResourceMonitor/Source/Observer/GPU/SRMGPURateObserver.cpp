
#include "Observer/GPU/SRMGPURateObserver.h"
#include "Observer/SRMModuleSubject.h"
#include "Observer/GPU/Nvidia/SRMNvidiaGPUInfo.h"

REG_SRM_OBSERVER(SRMGPURateObserver);
SRMGPURateObserver::SRMGPURateObserver()
	: SRMObserverBase("SRMGPURateObserver")
	, m_enGPUType(egtNune)
{

}

SRMGPURateObserver::~SRMGPURateObserver()
{

}

bool SRMGPURateObserver::init()
{
	if (SRMNvidiaGPUInfo::getInstance()->isValid())
	{
		m_enGPUType = egtNvidia;
		return true;
	}

	return false;
}

void SRMGPURateObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurGPURate = getGPURate();

	static VSShareMemTextNode oValueNode(0, 0, 0, L"0%");
	QColor oCurColor = fillCollorByValue(nCurGPURate, &oValueNode);
	oValueNode.m_sInfo = QString("%1%").arg(nCurGPURate, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"GPU");
	fillCollorByValue(nCurGPURate, &oNameNode);
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString("GPU:%1%").arg(nCurGPURate, 2, 10, QChar(' ')));
}

void SRMGPURateObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"GPU使用"));
}

int SRMGPURateObserver::order()
{
	return 40;
}

QString SRMGPURateObserver::customSettingDescription()
{
	return QString::fromStdWString(L"GPU使用:");
}

int SRMGPURateObserver::getGPURate()
{
	if (m_enGPUType == egtNvidia)
	{
		DISPLAY_INFO oInfo;
		SRMNvidiaGPUInfo::getInstance()->GetDisplayInfo(0, &oInfo);
		return oInfo.nGpuUsages[0];
	}


	return 0;
}
