
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QAction>
#include <QColor>
#include <QColorDialog>
#include <QIntValidator>
#include <QPushButton>
#include "UI/SRMCustomSettingDialog.h"
#include "Observer/SRMModuleSubject.h"
#include "Observer/SRMObserverBase.h"

SRMCustomSettingDialog::SRMCustomSettingDialog(QWidget* pParent)
	: QDialog(pParent)
{
	initLayout();
	initProp();
}

SRMCustomSettingDialog::~SRMCustomSettingDialog()
{

}

void SRMCustomSettingDialog::initProp()
{
	this->setFixedWidth(570);
	this->setWindowTitle(QString::fromStdWString(L"SRMResourceMonitor 自定义颜色设置"));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setAttribute(Qt::WA_DeleteOnClose, false);
	this->setAttribute(Qt::WA_QuitOnClose, false);
	this->setWindowIcon(QIcon(":/Images/SysResourceMonitor.png"));
}

void SRMCustomSettingDialog::initLayout()
{	
	QVBoxLayout* pScrollAreaLayout = new QVBoxLayout;
	QList<ISRMCustomSettingInf*>& oCustomSettingInfList = SRMModuleSubject::getInstance()->doCustomSettingInfList();
	foreach (ISRMCustomSettingInf * p, oCustomSettingInfList)
	{
		SRMCustomSettingItem* pItem = new SRMCustomSettingItem(p, this);
		pScrollAreaLayout->addWidget(pItem);
	}
	
	QWidget* pScrollWidget = new QWidget(this);
	pScrollWidget->setLayout(pScrollAreaLayout);

	QScrollArea* pScrollArea = new QScrollArea(this);
	pScrollArea->setWidget(pScrollWidget);
	pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout* pMainLayout = new QVBoxLayout;
	pMainLayout->addWidget(pScrollArea);
	this->setLayout(pMainLayout);
}

SRMCustomSettingItem::SRMCustomSettingItem(ISRMCustomSettingInf* pCustomSettingInf, QWidget* pParent)
	: QFrame(pParent)
	, m_pCustomSettingInf(pCustomSettingInf)
{
	initLayout();
	initProp();
}

SRMCustomSettingItem::~SRMCustomSettingItem()
{

}

void SRMCustomSettingItem::initLayout()
{
	auto createAction = [&](int nData, QColor & oColor)->QPushButton *
	{
		QPushButton* pPushButton = new QPushButton(this);
		pPushButton->setProperty("colorID", nData);
		pPushButton->setAutoFillBackground(true);
		pPushButton->setFixedWidth(100);
		pPushButton->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(oColor.red()).arg(oColor.green()).arg(oColor.blue()));
		bool bSuccess = connect(pPushButton, &QPushButton::clicked, this, &SRMCustomSettingItem::colorPushbuttonClickedSlot, Qt::UniqueConnection);
		return pPushButton;
	};

	QLabel* pLabelDescription = new QLabel(m_pCustomSettingInf->customSettingDescription(), this);

	QHBoxLayout* pSetttingLayout = new QHBoxLayout;

	QLabel* pLabelMin = new QLabel("0", this);
	pSetttingLayout->addWidget(pLabelMin);

	pSetttingLayout->addWidget(createAction(1, m_pCustomSettingInf->color1()));

	QIntValidator* pIntValidator = new QIntValidator(this);
	QLineEdit* pLineEditV1 = new QLineEdit(QString::number(m_pCustomSettingInf->v1()), this);
	pLineEditV1->setValidator(pIntValidator);
	pLineEditV1->setStyleSheet("color:black");
	pSetttingLayout->addWidget(pLineEditV1);
	connect(pLineEditV1, &QLineEdit::textEdited, this, &SRMCustomSettingItem::v1ValueChangedSlot, Qt::UniqueConnection);

	pSetttingLayout->addWidget(createAction(2, m_pCustomSettingInf->color2()));

	QLineEdit* pLineEditV2 = new QLineEdit(QString::number(m_pCustomSettingInf->v2()), this);
	pLineEditV2->setValidator(pIntValidator);
	pLineEditV2->setStyleSheet("color:black");
	pSetttingLayout->addWidget(pLineEditV2);
	connect(pLineEditV2, &QLineEdit::textEdited, this, &SRMCustomSettingItem::v2ValueChangedSlot, Qt::UniqueConnection);

	pSetttingLayout->addWidget(createAction(3, m_pCustomSettingInf->color3()));

	QLabel* pLabelMax = new QLabel("MAX", this);
	pSetttingLayout->addWidget(pLabelMax);

	QVBoxLayout* pMainLayout = new QVBoxLayout;
	pMainLayout->addWidget(pLabelDescription);
	pMainLayout->addLayout(pSetttingLayout);

	this->setLayout(pMainLayout);
}

void SRMCustomSettingItem::initProp()
{
	this->setFixedWidth(500);
	this->setFrameShape(QFrame::Box);
}

void SRMCustomSettingItem::colorPushbuttonClickedSlot()
{
	QPushButton* pPushbutton = dynamic_cast<QPushButton*>(sender());
	if (!pPushbutton)
		return;

	QColor oColor;
	switch (pPushbutton->property("colorID").toInt())
	{
	case 1:
		oColor = m_pCustomSettingInf->color1();
		break;
	case 2:
		oColor = m_pCustomSettingInf->color2();
		break;
	case 3:
		oColor = m_pCustomSettingInf->color3();
		break;
	default:
		break;
	}

	SRMColorDialog oColorDialog(oColor);
	oColorDialog.exec();
	oColor = oColorDialog.currentColor();

	switch (pPushbutton->property("colorID").toInt())
	{
	case 1:
		m_pCustomSettingInf->setColor1(oColor);
		break;
	case 2:
		m_pCustomSettingInf->setColor2(oColor);
		break;
	case 3:
		m_pCustomSettingInf->setColor3(oColor);
		break;
	default:
		break;
	}

	pPushbutton->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(oColor.red()).arg(oColor.green()).arg(oColor.blue()));
}

void SRMCustomSettingItem::v1ValueChangedSlot(QString sValue)
{
	QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
	if (!pLineEdit)
		return;

	auto setReject = [&]()->void
	{
		pLineEdit->setStyleSheet("color:red");
	};

	auto setAccept = [&]()->void
	{
		pLineEdit->setStyleSheet("color:black");
	};

	if (sValue.isEmpty())
	{
		setReject();
		return;
	}

	int nValue = sValue.toInt();
	if (nValue < 0)
	{
		setReject();
		return;
	}

	if (nValue > m_pCustomSettingInf->v2())
	{
		setReject();
		return;
	}

	m_pCustomSettingInf->setV1(nValue);
	setAccept();
}

void SRMCustomSettingItem::v2ValueChangedSlot(QString sValue)
{
	QLineEdit* pLineEdit = dynamic_cast<QLineEdit*>(sender());
	if (!pLineEdit)
		return;

	auto setReject = [&]()->void
	{
		pLineEdit->setStyleSheet("color:red");
	};

	auto setAccept = [&]()->void
	{
		pLineEdit->setStyleSheet("color:black");
	};

	if(sValue.isEmpty())
	{
		setReject();
		return;
	}

	int nValue = sValue.toInt();
	if (nValue < m_pCustomSettingInf->v1())
	{
		setReject();
		return;
	}

	m_pCustomSettingInf->setV2(nValue);
	setAccept();
}

SRMColorDialog::SRMColorDialog(QColor& oColor, QWidget* pParent /*= nullptr*/)
	: QColorDialog(oColor, pParent)
{
	this->setAttribute(Qt::WA_QuitOnClose, false);
}

SRMColorDialog::~SRMColorDialog()
{

}
