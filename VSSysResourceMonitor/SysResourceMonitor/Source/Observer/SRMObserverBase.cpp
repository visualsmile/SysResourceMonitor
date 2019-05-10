#include "Observer/SRMObserverBase.h"
#include "SRMConfig.h"
#include <QAction>
#include <QMenu>
#include <QStringList>

SRMObserverBase::SRMObserverBase(QString sObserverClassName)
	: m_sObserverClassName(sObserverClassName)
	, m_nV1(50)
	, m_nV2(75)
	, m_oColor1(0, 255, 0)
	, m_oColor2(255, 128, 0)
	, m_oColor3(255, 0, 0)
	, m_bIsChecked(true)
{
	readConfig();
}

SRMObserverBase::~SRMObserverBase()
{

}

bool SRMObserverBase::isChecked()
{
	return m_bIsChecked;
}

void SRMObserverBase::setIsChecked(bool bChecked)
{
	m_bIsChecked = bChecked;
	SRMConfig::getInstance()->saveConfig(m_sObserverClassName, "checked", bChecked);
}

void SRMObserverBase::actionStateChanged()
{
	QAction* pAction = dynamic_cast<QAction*>(sender());
	if (!pAction)
		return;

	if (isChecked())
		pAction->setIcon(QIcon(/*":/Images/UnCheck.png"*/));
	else
		pAction->setIcon(QIcon(":/Images/Check.png"));

	setIsChecked(!isChecked());
}

void SRMObserverBase::regTrayMenu(QMenu* pTrayMenu, QString sTrayActionName)
{
	QAction* pAction = new QAction(isChecked()
		? QIcon(":/Images/Check.png") : QIcon(/*":/Images/UnCheck.png"*/), sTrayActionName, this);
	pTrayMenu->addAction(pAction);
	connect(pAction, &QAction::triggered, this, &SRMObserverBase::actionStateChanged, Qt::UniqueConnection);
}

int SRMObserverBase::v1()
{
	return m_nV1;
}

int SRMObserverBase::v2()
{
	return m_nV2;
}

QColor SRMObserverBase::color1()
{
	return m_oColor1;
}

QColor SRMObserverBase::color2()
{
	return m_oColor2;
}

QColor SRMObserverBase::color3()
{
	return m_oColor3;
}

void SRMObserverBase::setV1(int nV)
{
	m_nV1 = nV;
	SRMConfig::getInstance()->saveConfig(m_sObserverClassName, "v1", nV);
}

void SRMObserverBase::setV2(int nV)
{
	m_nV2 = nV;
	SRMConfig::getInstance()->saveConfig(m_sObserverClassName, "v2", nV);
}

void SRMObserverBase::setColor1(QColor &oColor)
{
	m_oColor1 = oColor;
	SRMConfig::getInstance()->saveConfig(m_sObserverClassName, "color1", QString("%1,%2,%3,%4")
		.arg(oColor.red())
		.arg(oColor.green())
		.arg(oColor.blue())
		.arg(oColor.alpha()));
}

void SRMObserverBase::setColor2(QColor& oColor)
{
	m_oColor2 = oColor;
	SRMConfig::getInstance()->saveConfig(m_sObserverClassName, "color2", QString("%1,%2,%3,%4")
		.arg(oColor.red())
		.arg(oColor.green())
		.arg(oColor.blue())
		.arg(oColor.alpha()));
}

void SRMObserverBase::setColor3(QColor& oColor)
{
	m_oColor3 = oColor;
	SRMConfig::getInstance()->saveConfig(m_sObserverClassName, "color3", QString("%1,%2,%3,%4")
		.arg(oColor.red())
		.arg(oColor.green())
		.arg(oColor.blue())
		.arg(oColor.alpha()));
}

QColor SRMObserverBase::fillCollorByValue(int nValue, VSShareMemTextNode* pValueNode)
{
	QColor oColor;
	if (nValue < m_nV1)
		oColor = m_oColor1;
	else if (nValue < m_nV2)
		oColor = m_oColor2;
	else
		oColor = m_oColor3;

	pValueNode->m_nRed = oColor.red();
	pValueNode->m_nGreen = oColor.green();
	pValueNode->m_nBlue = oColor.blue();

	return oColor;
}

void SRMObserverBase::readConfig()
{
	QJsonValue oResult = SRMConfig::getInstance()->readConfig(m_sObserverClassName, "v1");
	if (!oResult.isNull())
		m_nV1 = oResult.toInt();

	oResult = SRMConfig::getInstance()->readConfig(m_sObserverClassName, "v2");
	if (!oResult.isNull())
		m_nV2 = oResult.toInt();

	oResult = SRMConfig::getInstance()->readConfig(m_sObserverClassName, "color1");
	if (!oResult.isNull())
	{
		QStringList oList = oResult.toString().split(",");
		m_oColor1 = QColor(oList[0].toInt(), oList[1].toInt(), oList[2].toInt(), oList[3].toInt());
	}

	oResult = SRMConfig::getInstance()->readConfig(m_sObserverClassName, "color2");
	if (!oResult.isNull())
	{
		QStringList oList = oResult.toString().split(",");
		m_oColor2 = QColor(oList[0].toInt(), oList[1].toInt(), oList[2].toInt(), oList[3].toInt());
	}

	oResult = SRMConfig::getInstance()->readConfig(m_sObserverClassName, "color3");
	if (!oResult.isNull())
	{
		QStringList oList = oResult.toString().split(",");
		m_oColor3 = QColor(oList[0].toInt(), oList[1].toInt(), oList[2].toInt(), oList[3].toInt());
	}

	oResult = SRMConfig::getInstance()->readConfig(m_sObserverClassName, "checked");
	if (!oResult.isNull())
		m_bIsChecked = oResult.toBool();
}
