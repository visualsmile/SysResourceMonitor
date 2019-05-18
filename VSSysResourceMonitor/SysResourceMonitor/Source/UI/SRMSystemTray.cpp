
#include "UI/SRMSystemTray.h"
#include "UI/SRMFloatingWidget.h"
#include "SRMRegManager.h"
#include "SRMConfig.h"
#include "VSDeskBandOperation.h"
#include "SysResourceMonitor.h"
#include "Observer/SRMModuleSubject.h"
#include <QMenu>
#include <QWidgetAction>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <UI/Common/VSCustomMenu.h>
#include "SRMServiceOperation.h"
#include "UI/SRMCustomSettingDialog.h"
#include "UI/SRMHelpDialog.h"

const QString c_sSetAutoRun = QString::fromStdWString(L"开机自启");
const QString c_sCancelAutoRun = QString::fromStdWString(L"取消自启");
const QString c_sFloatingWidgetShowMode = QString::fromStdWString(L"浮窗模式");
const QString c_sHideFloatingWidget = QString::fromStdWString(L"隐藏浮窗");
const QString c_sShowWidgetNorm = QString::fromStdWString(L"普通窗口模式");
const QString c_sShowWidgetNormal_EdgeAutoHide = QString::fromStdWString(L"贴边自动隐藏");
const QString c_sShowWidgetFixed = QString::fromStdWString(L"固定不可捕捉");
const QString c_sShowWidgetFixed_SlippingAutoHide = QString::fromStdWString(L"触碰自动隐藏");
const QString c_sShowWidgetHide_SlippingAutoShow = QString::fromStdWString(L"始终自动隐藏");
const QString c_sFloatingWdigetPosition = QString::fromStdWString(L"浮窗位置");
const QString c_sAttachTopMid = QString::fromStdWString(L"中上位置");
const QString c_sAttachTopLeft = QString::fromStdWString(L"左上位置");
const QString c_sAttachTopRight = QString::fromStdWString(L"右上位置");
const QString c_sAttachBottomMid = QString::fromStdWString(L"中下位置");
const QString c_sAttachBottomLeft = QString::fromStdWString(L"左下位置");
const QString c_sAttachBottomRight = QString::fromStdWString(L"右下位置");
const QString c_sSellectAll = QString::fromStdWString(L"全部选中");
const QString c_sCancelAll = QString::fromStdWString(L"全部移除");
const QString c_sDeskBandOperation = QString::fromStdWString(L"标尺管理");
const QString c_sUpdateDeskBand = QString::fromStdWString(L"刷新标尺");
const QString c_sShowDeskBand = QString::fromStdWString(L"打开标尺");
const QString c_sHideDeskBand = QString::fromStdWString(L"关闭标尺");
const QString c_sRegDeskBand = QString::fromStdWString(L"注册标尺");
const QString c_sUnregDeskBand = QString::fromStdWString(L"注销标尺");
const QString c_sServiceOperation = QString::fromStdWString(L"服务管理");
const QString c_sinstallService = QString::fromStdWString(L"安装服务");
const QString c_sUninstallService = QString::fromStdWString(L"卸载服务");
const QString c_sStartService = QString::fromStdWString(L"启动服务");
const QString c_sStopService = QString::fromStdWString(L"停止服务");
const QString c_sQuit = QString::fromStdWString(L"退出");
const QString c_sCustomSetting = QString::fromStdWString(L"自定义颜色");
const QString c_sHelp = QString::fromStdWString(L"帮助");

SRMSystemTray::SRMSystemTray(SRMFloatingWidget* pFloatingWidget)
	: m_pFloatingWidget(pFloatingWidget)
{
	m_pTrayMenu = new VSCustomMenu(nullptr);
	createAutoRunMenu(m_pTrayMenu);

	m_pTrayMenu->addSeparator();
	createOptionMenu(m_pTrayMenu);

	m_pTrayMenu->addSeparator();
	createFloatingWidgetModelMenu(m_pTrayMenu);
	createFloatingWidgetPositionMenu(m_pTrayMenu);
	createDeskBandMenu(m_pTrayMenu);
	createServiceMenu(m_pTrayMenu);
	createCustomMenu(m_pTrayMenu);

	m_pTrayMenu->addSeparator();
	createHelpMenu(m_pTrayMenu);

	m_pTrayMenu->addSeparator();
	createQuitMenu(m_pTrayMenu);
	
	//托盘提示信息、图标
	m_pSystemTray = new QSystemTrayIcon(this);
	m_pSystemTray->setToolTip(QString("SysResourceMonitor"));
	m_pSystemTray->setIcon(QIcon(":/Images/SysResourceMonitor.png"));
	m_pSystemTray->setContextMenu(m_pTrayMenu);
}

SRMSystemTray::~SRMSystemTray()
{
	if (m_pTrayMenu)
		delete m_pTrayMenu;
}

void SRMSystemTray::show()
{
	m_pSystemTray->show();
}

void SRMSystemTray::hide()
{
	m_pSystemTray->hide();
}

void SRMSystemTray::createAutoRunMenu(QMenu* pTrayMenu)
{
	bool bAutoRun = SRMRegManager::getInstance()->isAutoRun();
	QAction* pAutoRunAction = new QAction(QIcon(":/Images/AutoRun.png"), bAutoRun ? c_sCancelAutoRun : c_sSetAutoRun, pTrayMenu);
	pTrayMenu->addAction(pAutoRunAction);
	connect(pAutoRunAction, &QAction::triggered, this, &SRMSystemTray::autoRunChangedSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createFloatingWidgetModelMenu(QMenu* pTrayMenu)
{
	QMenu* pFloatingWidgetModeMenu = new QMenu(c_sFloatingWidgetShowMode, pTrayMenu);
	pFloatingWidgetModeMenu->setIcon(QIcon(":/Images/WidgetMode.png"));

	// 窗口显示模式
	EnVSFWShowMode enRegShowMode = m_pFloatingWidget->getShowMode();	
	auto createAction = [&](EnVSFWShowMode enDestMode, const QString &sName) -> QAction *
	{
		QAction* pAction = new QAction(enRegShowMode == enDestMode
			? QIcon(":/Images/Check.png") : QIcon(/*":/Images/UnCheck.png"*/), sName, pFloatingWidgetModeMenu);
		pAction->setData(int(enDestMode));
		return pAction;
	};

	pFloatingWidgetModeMenu->addAction(createAction(esmHide, c_sHideFloatingWidget));
	pFloatingWidgetModeMenu->addAction(createAction(esmNormal, c_sShowWidgetNorm));
	pFloatingWidgetModeMenu->addAction(createAction(esmNormal_EdgeAutoHide, c_sShowWidgetNormal_EdgeAutoHide));
	pFloatingWidgetModeMenu->addAction(createAction(esmShowFixed, c_sShowWidgetFixed));
	pFloatingWidgetModeMenu->addAction(createAction(esmShowFixed_SlippingAutoHide, c_sShowWidgetFixed_SlippingAutoHide));
	pFloatingWidgetModeMenu->addAction(createAction(esmHide_SlippingAutoShow, c_sShowWidgetHide_SlippingAutoShow));

	pTrayMenu->addMenu(pFloatingWidgetModeMenu);
	connect(pFloatingWidgetModeMenu, &QMenu::triggered, this, &SRMSystemTray::floatingWidgetModeMenuSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createFloatingWidgetPositionMenu(QMenu* pTrayMenu)
{
	QMenu* pFloatingWidgetPositionMenu = new QMenu(c_sFloatingWdigetPosition, pTrayMenu);
	pFloatingWidgetPositionMenu->setIcon(QIcon(":/Images/WidgetPosition.png"));

	auto createAction = [&](const QString & sName, EnVSFWPositionType enDestType) -> QAction *
	{
		QAction* pAction = new QAction(sName, pFloatingWidgetPositionMenu);
		pAction->setData(int(enDestType));
		return pAction;
	};

	pFloatingWidgetPositionMenu->addAction(createAction(c_sAttachTopLeft, eptTopLeft));
	pFloatingWidgetPositionMenu->addAction(createAction(c_sAttachTopMid, eptTopMid));	
	pFloatingWidgetPositionMenu->addAction(createAction(c_sAttachTopRight, eptTopRight));
	pFloatingWidgetPositionMenu->addAction(createAction(c_sAttachBottomLeft, eptBottomLeft));
	pFloatingWidgetPositionMenu->addAction(createAction(c_sAttachBottomMid, eptBottomMid));	
	pFloatingWidgetPositionMenu->addAction(createAction(c_sAttachBottomRight, eptBottomRight));

	pTrayMenu->addMenu(pFloatingWidgetPositionMenu);
	connect(pFloatingWidgetPositionMenu, &QMenu::triggered, this, &SRMSystemTray::floatingWidgetPositionMenuSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createDeskBandMenu(QMenu* pTrayMenu)
{
	QMenu* pDeskBandOperationMenu = new QMenu(c_sDeskBandOperation, pTrayMenu);
	pDeskBandOperationMenu->setIcon(QIcon(":/Images/DeskBandSetting.png"));

	pDeskBandOperationMenu->addAction(new QAction(c_sUpdateDeskBand, pDeskBandOperationMenu));
	pDeskBandOperationMenu->addAction(new QAction(c_sShowDeskBand, pDeskBandOperationMenu));
	pDeskBandOperationMenu->addAction(new QAction(c_sHideDeskBand, pDeskBandOperationMenu));
	pDeskBandOperationMenu->addAction(new QAction(c_sRegDeskBand, pDeskBandOperationMenu));
	pDeskBandOperationMenu->addAction(new QAction(c_sUnregDeskBand, pDeskBandOperationMenu));

	pTrayMenu->addMenu(pDeskBandOperationMenu);
	connect(pDeskBandOperationMenu, &QMenu::triggered, this, &SRMSystemTray::deskBandOperationChangedSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createOptionMenu(QMenu* pTrayMenu)
{
	SRMModuleSubject::getInstance()->doRegTrayMenu(pTrayMenu);
}

void SRMSystemTray::createServiceMenu(QMenu* pTrayMenu)
{
	QMenu* pServiceOperationMenu = new QMenu(c_sServiceOperation, pTrayMenu);
	pServiceOperationMenu->setIcon(QIcon(":/Images/ServiceSetting.png"));

	pServiceOperationMenu->addAction(new QAction(c_sinstallService, pServiceOperationMenu));
	pServiceOperationMenu->addAction(new QAction(c_sUninstallService, pServiceOperationMenu));
	pServiceOperationMenu->addAction(new QAction(c_sStartService, pServiceOperationMenu));
	pServiceOperationMenu->addAction(new QAction(c_sStopService, pServiceOperationMenu));

	pTrayMenu->addMenu(pServiceOperationMenu);
	connect(pServiceOperationMenu, &QMenu::triggered, this, &SRMSystemTray::serviceOperationChangedSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createCustomMenu(QMenu* pTrayMenu)
{
	QAction* pAction = new QAction(QIcon(":/Images/CustomSetting.png"), c_sCustomSetting, pTrayMenu);
	pTrayMenu->addAction(pAction);
	connect(pAction, &QAction::triggered, this, &SRMSystemTray::customSettingSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createHelpMenu(QMenu* pTrayMenu)
{
	QAction* pHelpAction = new QAction(QIcon(":/Images/Help.png"), c_sHelp, pTrayMenu);
	pTrayMenu->addAction(pHelpAction);
	connect(pHelpAction, &QAction::triggered, this, &SRMSystemTray::helpSlot, Qt::UniqueConnection);
}

void SRMSystemTray::createQuitMenu(QMenu* pTrayMenu)
{
	QAction* pQuitAction = new QAction(QIcon(":/Images/Quit.png"), c_sQuit, pTrayMenu);
	pTrayMenu->addAction(pQuitAction);
	connect(pQuitAction, &QAction::triggered, this, &SRMSystemTray::quitSlot, Qt::UniqueConnection);
}

void SRMSystemTray::autoRunChangedSlot()
{
	QAction* pAction = dynamic_cast<QAction*>(sender());
	if (!pAction)
		return;

	if (pAction->text() == c_sSetAutoRun)
	{
		if (!SRMRegManager::getInstance()->setAutoRun())
			return;

		pAction->setText(c_sCancelAutoRun);		
	}
	else
	{
		if (!SRMRegManager::getInstance()->cancelAutoRun())
			return;

		pAction->setText(c_sSetAutoRun);		
	}
}

void SRMSystemTray::floatingWidgetModeMenuSlot(QAction* pAction)
{
	QMenu* pParent = dynamic_cast<QMenu*>(pAction->parent());
	if (!pParent)
		return;

	QObjectList oList = pParent->children();
	foreach(QObject * p, oList)
	{
		QAction* pAction = dynamic_cast<QAction*>(p);
		if (!pAction)
			continue;

		if (pAction == pParent->menuAction())
			continue;

		pAction->setIcon(QIcon(/*":/Images/UnCheck.png"*/));
	}

	pAction->setIcon(QIcon(":/Images/Check.png"));
	m_pFloatingWidget->setShowMode(EnVSFWShowMode(pAction->data().toInt()));
}

void SRMSystemTray::floatingWidgetPositionMenuSlot(QAction* pAction)
{
	EnVSFWPositionType enType = EnVSFWPositionType(pAction->data().toInt());
	m_pFloatingWidget->changePositionType(enType);
}

void SRMSystemTray::deskBandOperationChangedSlot(QAction *pAction)
{
	m_pFloatingWidget->pauseTopOn();

	auto updateDeskBand = [&]() ->void
	{
		hideDeskBand();
		showDeskBand();
	};
	
	if (pAction->text() == c_sHideDeskBand)
		hideDeskBand();
	else if (pAction->text() == c_sShowDeskBand)
		showDeskBand();
	else if (pAction->text() == c_sRegDeskBand)
		VSDeskBandOperation::getInstance()->doReg();
	else if (pAction->text() == c_sUnregDeskBand)
		VSDeskBandOperation::getInstance()->doUnreg();
	else
		updateDeskBand();

	m_pFloatingWidget->resumeTopOn();
}

void SRMSystemTray::serviceOperationChangedSlot(QAction* pAction)
{
	m_pFloatingWidget->pauseTopOn();
	if (pAction->text() == c_sinstallService)
		SRMServiceOperation::getInstance()->regService();
	else if (pAction->text() == c_sUninstallService)
		SRMServiceOperation::getInstance()->unregService();
	else if (pAction->text() == c_sStartService)
		SRMServiceOperation::getInstance()->startService();
	else if (pAction->text() == c_sStopService)
		SRMServiceOperation::getInstance()->stopService();
	m_pFloatingWidget->resumeTopOn();
}

void SRMSystemTray::quitSlot()
{
	qApp->quit();
}

void SRMSystemTray::customSettingSlot()
{	
	static QDialog* pDialog = new SRMCustomSettingDialog(nullptr);
	if (!pDialog->isHidden())
		return;

	m_pFloatingWidget->pauseTopOn();
	pDialog->exec();
	m_pFloatingWidget->resumeTopOn();
}

void SRMSystemTray::helpSlot()
{
	static QDialog* pDialog = new SRMHelpDialog(nullptr);
	if (!pDialog->isHidden())
		return;

	m_pFloatingWidget->pauseTopOn();
	pDialog->exec();
	m_pFloatingWidget->resumeTopOn();
}

bool SRMSystemTray::showDeskBand()
{
	if (!VSDeskBandOperation::getInstance()->isRegistered())
		if (!VSDeskBandOperation::getInstance()->doReg())
			return false;

	VSDeskBandOperation::getInstance()->show(true);
	SRMConfig::getInstance()->saveConfig("DeskBand", "ShowDeskBand", true);
	return true;
}

void SRMSystemTray::hideDeskBand()
{
	VSDeskBandOperation::getInstance()->hide();
	SRMConfig::getInstance()->saveConfig("DeskBand", "ShowDeskBand", false);
}
