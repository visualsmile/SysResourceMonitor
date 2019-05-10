#include "UI/SRMFloatingWidget.h"
#include "SRMConfig.h"
#include "Observer/SRMModuleSubject.h"
#include <QLabel>
#include <QHBoxLayout>

SRMFloatingWidget::SRMFloatingWidget(QWidget* pParent /*= nullptr*/)
{
	QPoint oPoint(0, 0);
	QJsonValue oResult = SRMConfig::getInstance()->readConfig("SRMFloatingWidget", "CurPosition");
	if (!oResult.isNull())
	{
		QStringList oList = oResult.toString().split(",");
		oPoint = QPoint(oList[0].toInt(), oList[1].toInt());
	}

	EnVSFWPositionType enPositionType = eptTopMid;
	oResult = SRMConfig::getInstance()->readConfig("SRMFloatingWidget", "PositionType");
	if (!oResult.isNull())
		enPositionType = EnVSFWPositionType(oResult.toInt());

	EnVSFWShowMode enShowModel = esmShowFixed;
	oResult = SRMConfig::getInstance()->readConfig("SRMFloatingWidget", "ShowMode");
	if (!oResult.isNull())
		enShowModel = EnVSFWShowMode(oResult.toInt());

	this->updateLayout();
	this->move(oPoint);
	__super::setShowMode(enShowModel);
}
 
SRMFloatingWidget::~SRMFloatingWidget()
{
	saveConfig();
}

QList<QLabel*> SRMFloatingWidget::updateLayout()
{
	QLayout* pMainLyout = this->layout();
	if (!pMainLyout)
	{
		pMainLyout = new QHBoxLayout;
		pMainLyout->setContentsMargins(5, 0, 5, 0);
		pMainLyout->setSizeConstraint(QLayout::SetFixedSize);  // 设置布局自动适应控件变化
	}

	foreach(QLabel * p, m_oLabelList)
	{
		pMainLyout->removeWidget(p);
		delete p;
	}
	m_oLabelList.clear();

	int nCount = SRMModuleSubject::getInstance()->getCheckedCount();
	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		QLabel* pLabel = new QLabel(this);
		pMainLyout->addWidget(pLabel);
		m_oLabelList.push_back(pLabel);
	}

	if (m_oLabelList.size() == 0)
	{
		QLabel* pEmptyLabel = new QLabel("", this);
		pMainLyout->addWidget(pEmptyLabel);
	}

	this->setLayout(pMainLyout);

	return m_oLabelList;
}

void SRMFloatingWidget::changePositionType(EnVSFWPositionType enType)
{
	this->move(__super::calcPosition(enType));
	saveConfig();
}

void SRMFloatingWidget::setShowMode(EnVSFWShowMode enShowMode)
{
	__super::setShowMode(enShowMode);
	saveConfig();
}

void SRMFloatingWidget::saveConfig()
{
	SRMConfig::getInstance()->saveConfig("SRMFloatingWidget", "ShowMode", this->getShowMode());
	QPoint oCurPosition = this->pos();
	SRMConfig::getInstance()->saveConfig("SRMFloatingWidget", "CurPosition", QString("%1,%2")
		.arg(oCurPosition.x())
		.arg(oCurPosition.y()));
}
