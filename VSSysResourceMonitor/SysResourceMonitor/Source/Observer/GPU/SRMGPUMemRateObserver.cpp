
#include "Observer/GPU/SRMGPUMemRateObserver.h"
#include "Observer/SRMModuleSubject.h"
#include "Observer/GPU/Nvidia/SRMNvidiaGPUInfo.h"

REG_SRM_OBSERVER(SRMGPUMemRateObserver);
SRMGPUMemRateObserver::SRMGPUMemRateObserver()
	: SRMObserverBase("SRMGPUMemRateObserver")
	, m_pGPUInfoInf(nullptr)
{

}

SRMGPUMemRateObserver::~SRMGPUMemRateObserver()
{

}

bool SRMGPUMemRateObserver::init()
{
	m_pGPUInfoInf = SRMNvidiaGPUInfo::createGPUInfoInf();
	if (!m_pGPUInfoInf)
		return false;

	return true;
}

void SRMGPUMemRateObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurGPUMemRate = m_pGPUInfoInf->getMemRate(0);

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

