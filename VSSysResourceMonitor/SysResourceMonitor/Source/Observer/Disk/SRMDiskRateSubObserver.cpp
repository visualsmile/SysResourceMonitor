#include "Observer/Disk/SRMDiskRateSubObserver.h"

SRMDiskRateSubObserver::SRMDiskRateSubObserver(QString sPartitionName, QString sTrayMenuName)
	: SRMObserverBase("SRMDiskRateSubObserver_" + sTrayMenuName)
	, m_sPartitionName(sPartitionName)
	, m_sTrayMenuName(sTrayMenuName)
	, m_nRate(0)
{

}

SRMDiskRateSubObserver::~SRMDiskRateSubObserver()
{

}

bool SRMDiskRateSubObserver::init()
{
	return true;
}

void SRMDiskRateSubObserver::update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList)
{
	static VSShareMemTextNode oValueNode(0, 0, 0, L"0%");
	QColor oCurColor = fillCollorByValue(m_nRate, &oValueNode);
	oValueNode.m_sInfo = QString("%1%").arg(m_nRate, 2, 10, QChar(' ')).toStdWString();
	pSharedMemStruct->setTextNode(0, nIndex, oValueNode);

	static VSShareMemTextNode oNameNode(0, 0, 0, L"");
	fillCollorByValue(m_nRate, &oNameNode);
	oNameNode.m_sInfo = L" " + m_sPartitionName.toStdWString() + L" ";
	pSharedMemStruct->setTextNode(1, nIndex, oNameNode);

	static QPalette oPalette = oLabelList[nIndex]->palette();
	oPalette.setColor(QPalette::WindowText, oCurColor);
	oLabelList[nIndex]->setPalette(oPalette);
	oLabelList[nIndex]->setText(QString("%1%2%").arg(m_sPartitionName).arg(m_nRate, 2, 10, QChar(' ')));
}

void SRMDiskRateSubObserver::regTrayMenu(QMenu* pTrayMenu)
{
	__super::regTrayMenu(pTrayMenu, m_sTrayMenuName);
}

int SRMDiskRateSubObserver::order()
{
	return 0;
}

QString SRMDiskRateSubObserver::customSettingDescription()
{
	return QString::fromStdWString(L"¥≈≈Ã π”√:%1").arg(m_sTrayMenuName);
}

void SRMDiskRateSubObserver::updateInfo(QString sPartitionName, int nRate)
{
	m_sPartitionName = sPartitionName;
	m_nRate = nRate;
}

