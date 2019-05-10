
#include "Observer/Mem/SRMMemRateObserver.h"
#include "Observer/SRMModuleSubject.h"

REG_SRM_OBSERVER(SRMMemRateObserver);
SRMMemRateObserver::SRMMemRateObserver()
	: SRMObserverBase("SRMMemRateObserver")
{

}

SRMMemRateObserver::~SRMMemRateObserver()
{

}

bool SRMMemRateObserver::init()
{
	return true;
}

void SRMMemRateObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	int nCurMemRate = getMemRate();

	static VSShareMemTextNode oValueNode(0, 0, 0, L"0%");
	QColor oCurColor = fillCollorByValue(nCurMemRate, &oValueNode);
	oValueNode.m_sInfo = QString("%1%").arg(nCurMemRate, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"RAM");
	fillCollorByValue(nCurMemRate, &oNameNode);
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString("RAM:%1%").arg(nCurMemRate, 2, 10, QChar(' ')));
}

void SRMMemRateObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, QString::fromStdWString(L"内存使用"));
}

int SRMMemRateObserver::order()
{
	return 30;
}

QString SRMMemRateObserver::customSettingDescription()
{
	return QString::fromStdWString(L"内存使用:");
}

int SRMMemRateObserver::getMemRate()
{
	MEMORYSTATUSEX oStatex;
	oStatex.dwLength = sizeof(oStatex);
	GlobalMemoryStatusEx(&oStatex);
	return oStatex.dwMemoryLoad;
}
