
#include "SysResourceMonitor.h"
#include "UI/SRMFloatingWidget.h"
#include "Observer/SRMModuleSubject.h"
#include "VSDeskBandOperation.h"
#include "UI/SRMSystemTray.h"
#include <QHBoxLayout>

SysResourceMonitor::SysResourceMonitor(QWidget* parent /*= Q_NULLPTR*/)
	: QMainWindow(parent)
{
	this->setWindowFlags(this->windowFlags() | Qt::Tool);
}

SysResourceMonitor::~SysResourceMonitor()
{
	VSDeskBandOperation::getInstance()->hide();
	if (m_pFloatingWidget)
		delete m_pFloatingWidget;

	if (m_pSystemTray)
		delete m_pSystemTray;
}

void SysResourceMonitor::doWork()
{
	SRMModuleSubject::getInstance()->init();

	m_pVSSharedMemStruct = new VSSharedMemStruct;
	m_pVSSharedMemStruct->setRowCount(2);
	m_pVSSharedMemStruct->setColCount(0);

	m_pFloatingWidget = new SRMFloatingWidget;
	m_pFloatingWidget->show();

	m_pSystemTray = new SRMSystemTray(m_pFloatingWidget);
	m_pSystemTray->show();

	connect(&m_oTimer, &QTimer::timeout, this, &SysResourceMonitor::update, Qt::UniqueConnection);
	m_oTimer.start(1000);
}

void SysResourceMonitor::update()
{		
	updateWidget();
	SRMModuleSubject::getInstance()->doOntify(m_pVSSharedMemStruct, m_oLabelList);
	updateDeskBand();
}

void SysResourceMonitor::updateWidget()
{
	int nCount = SRMModuleSubject::getInstance()->getCheckedCount();
	if (nCount == m_oLabelList.size())
		return;

	m_oLabelList = m_pFloatingWidget->updateLayout();
}

void SysResourceMonitor::updateDeskBand()
{
	int nCount = SRMModuleSubject::getInstance()->getCheckedCount();
	if (nCount == m_pVSSharedMemStruct->getColCount())
	{
		VSDeskBandOperation::getInstance()->updateMessage(m_pVSSharedMemStruct);
	}
	else
	{
		m_pVSSharedMemStruct->setRowCount(2);
		m_pVSSharedMemStruct->setColCount(nCount);
		VSDeskBandOperation::getInstance()->updateMessage(m_pVSSharedMemStruct);

		bool bShow = true;
		QJsonValue oResult = SRMConfig::getInstance()->readConfig("DeskBand", "ShowDeskBan");
		if (!oResult.isNull())
			bShow = oResult.toBool();

		if (bShow)
		{
			VSDeskBandOperation::getInstance()->hide();
			VSDeskBandOperation::getInstance()->show(true);
		}
	}	
}

