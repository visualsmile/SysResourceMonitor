#include "VSFloatingWidget.h"
#include <QTimer>
#include <Windows.h>
#include <QLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QPainter>
#include <QHBoxLayout>
#include <QLabel>

VSFloatingWidget::VSFloatingWidget(EnVSFWShowMode enShowMode /*= enfwNormal*/, QPoint& oCurPosition /*= QPoint(0, 0)*/, QWidget* parent /*= nullptr*/)
	: m_enShowMode(enShowMode)
	, QWidget(parent)
	, m_oOldPos(0, 0)
	, m_oScreenRect(QApplication::desktop()->screenGeometry())
	, m_nPausedCount(0)
{
	//this->resize(800, 600);
	//QHBoxLayout* pLayout = new QHBoxLayout;
	//QLabel* pLabel = new QLabel;
	//QPalette oPalette;
	//oPalette.setColor(QPalette::WindowText, QColor(0, 255, 0));
	//pLabel->setPalette(oPalette);
	//pLabel->setText("1234567890");
	//pLayout->addWidget(pLabel);
	//this->setLayout(pLayout);

	initWidgetProperty();

	setSlidingSpeed();
	setAttachBoder();
	setWidgetOpacity();
	setBackgroundColor();
	setFrequency();
	this->move(oCurPosition);
}

VSFloatingWidget::~VSFloatingWidget()
{
	
}

void VSFloatingWidget::setShowMode(EnVSFWShowMode enShowMode)
{
	m_enShowMode = enShowMode;
	this->show();
}

EnVSFWShowMode VSFloatingWidget::getShowMode()
{
	return m_enShowMode;
}

void VSFloatingWidget::setAttachBoder(int nAttachBorder)
{
	if (nAttachBorder < 1)
		nAttachBorder = 1;

	m_nAttachBorder = nAttachBorder;
}

void VSFloatingWidget::setWidgetOpacity(double dOpacity)
{
	m_dWidgetOpacity = dOpacity;
	this->setWindowOpacity(dOpacity);
}

void VSFloatingWidget::setBackgroundColor(QColor& oColor /*= QColor(0, 0, 0)*/, double dAlpha /*= 0*/)
{
	m_oBackgroundColor = oColor;
	m_oBackgroundColor.setAlphaF(dAlpha);
}

void VSFloatingWidget::setFrequency(int nFrequence)
{
	m_oTimer.stop();
	m_oTimer.start(1000 / nFrequence);
	connect(&m_oTimer, &QTimer::timeout, this, &VSFloatingWidget::stackWidgetOnTop, Qt::UniqueConnection);
}

void VSFloatingWidget::show()
{
	if (m_enShowMode == esmHide)
	{
		this->hide();
		return;
	}

	__super::show();	

	if (m_enShowMode == esmShowFixed)
		setIgnoreAllEvent(true);
	else
		setIgnoreAllEvent(false);

	switch (m_enShowMode)
	{
	case esmShowFixed:
	case esmNormal:
	case esmNormal_EdgeAutoHide:
	case esmShowFixed_SlippingAutoHide:
		showWidgetSliding();
		break;
	case esmHide_SlippingAutoShow:
		hideWidgetSliding();
		break;
	default:
		break;
	}
}

void VSFloatingWidget::pauseTopOn()
{
	m_oTimer.stop();
	m_nPausedCount++;
}


void VSFloatingWidget::resumeTopOn()
{
	m_nPausedCount--;
	if (m_nPausedCount <= 0)
	{
		m_nPausedCount = 0;
		m_oTimer.start();
	}	
}

void VSFloatingWidget::setSlidingSpeed(int nHideSpeed /*= 2*/)
{
	if (nHideSpeed < 1)
		nHideSpeed = 1;

	m_nHideSpeed = nHideSpeed;
}

void VSFloatingWidget::mouseMoveEvent(QMouseEvent* event)
{
	switch (m_enShowMode)
	{
	case esmNormal:
	case esmNormal_EdgeAutoHide:
		if (event->buttons() == Qt::LeftButton)
		{
			QPoint nCurrentPos = event->globalPos() - m_oOldPos;

			if (nCurrentPos.x() + this->width() <= m_nAttachBorder)
			{
				nCurrentPos.setX(0);
			}
			else if (nCurrentPos.x() + m_nAttachBorder >= m_oScreenRect.width())
			{
				nCurrentPos.setX(m_oScreenRect.width() - this->width());
			}

			if (nCurrentPos.y() + this->height() <= m_nAttachBorder)
			{
				nCurrentPos.setY(0);
			}
			else if (nCurrentPos.y() + m_nAttachBorder >= m_oScreenRect.height())
			{
				nCurrentPos.setY(m_oScreenRect.height() - this->height());
			}

			this->move(nCurrentPos);
		}
		break;
	case esmHide:
	case esmShowFixed:
	case esmShowFixed_SlippingAutoHide:
	case esmHide_SlippingAutoShow:
	default:
		break;
	}

	__super::mouseMoveEvent(event);
}

void VSFloatingWidget::mousePressEvent(QMouseEvent* event)
{
	// 鼠标点击时更新屏幕信息
	m_oScreenRect = QApplication::desktop()->screenGeometry();

	switch (m_enShowMode)
	{
	case esmNormal_EdgeAutoHide:
	case esmNormal:
		m_oOldPos = event->globalPos() - this->pos();
		break;
	case esmHide:
	case esmShowFixed:
	case esmHide_SlippingAutoShow:
	case esmShowFixed_SlippingAutoHide:
	default:
		break;
	}

	__super::mousePressEvent(event);
}

void VSFloatingWidget::mouseReleaseEvent(QMouseEvent* event)
{
	switch (m_enShowMode)
	{
	case esmNormal:
	case esmNormal_EdgeAutoHide:	
		break;
	case esmHide:
	case esmHide_SlippingAutoShow:
	case esmShowFixed:
	case esmShowFixed_SlippingAutoHide:
	default:
		break;
	}

	__super::mousePressEvent(event);
}

void VSFloatingWidget::paintEvent(QPaintEvent* event)
{
	QPainter oPainter(this);
	oPainter.fillRect(this->rect(), m_oBackgroundColor);
	__super::paintEvent(event);
}

QPoint VSFloatingWidget::calcPosition(EnVSFWPositionType enAttachType)
{
	RECT oRect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &oRect, 0);
	int nTaskBarHeight = GetSystemMetrics(SM_CYSCREEN) - oRect.bottom;
	QRect oScreenRect = QApplication::desktop()->screenGeometry();

	QPoint oResultPoint(0, 0);
	switch (enAttachType)
	{
	case eptTopMid:
		oResultPoint = QPoint((oScreenRect.width() - this->width()) / 2.0, 0);
		break;
	case eptTopLeft:
		oResultPoint = QPoint(0, 0);
		break;
	case eptTopRight:
		oResultPoint = QPoint(oScreenRect.width() - this->width(), 0);
		break;
	case eptBottomMid:
		oResultPoint = QPoint((oScreenRect.width() - this->width()) / 2.0, oScreenRect.height() - this->height());
		break;
	case eptBottomLeft:
		oResultPoint = QPoint(0, nTaskBarHeight);
		break;
	case eptBottomRight:
		oResultPoint = QPoint(oScreenRect.width() - this->width(), nTaskBarHeight);
		break;
	default:
		break;
	}

	return oResultPoint;
}

void VSFloatingWidget::stackWidgetOnTop()
{
	if (this->isHidden())
		return;

	if (this->isMinimized())
		this->showNormal();

	RECT oRect;
	if (GetWindowRect((HWND)this->winId(), &oRect))
		SetWindowPos((HWND)this->winId(), HWND_TOPMOST, oRect.left, oRect.top, oRect.right-oRect.left, oRect.bottom-oRect.top, SWP_SHOWWINDOW);
}

void VSFloatingWidget::hideWidgetSliding()
{
	auto getShortestDirection = [&](int &nDistance) -> EnVSFWHideDirection
	{
		
		// 优先级： 左 > 右 > 上 > 下
		int nLeftDistance = this->pos().x();
		int nRightDistance =  m_oScreenRect.width() - (this->width() + this->pos().x());
		int nTopDistance = this->pos().y();
		int nBottomDistance = m_oScreenRect.height() - (this->pos().y() + this->height());

		EnVSFWHideDirection enResultR = nLeftDistance <= nRightDistance ? ehdLeft : ehdRight;
		int nDistanceR = nLeftDistance <= nRightDistance ? nLeftDistance : nRightDistance;
		EnVSFWHideDirection enResultC = nTopDistance <= nBottomDistance ? ehdTop : ehdBottom;
		int nDistanceC = nTopDistance <= nBottomDistance ? nTopDistance : nBottomDistance;

		EnVSFWHideDirection enResult = nDistanceR <= nDistanceC ? enResultR : enResultC;
		nDistance = nDistanceR <= nDistanceC ? nDistanceR : nDistanceC;
		return enResult;
	};

	switch (m_enShowMode)
	{
	case esmNormal:
	case esmShowFixed:
	case esmHide:
		break;
	case esmNormal_EdgeAutoHide:
	case esmShowFixed_SlippingAutoHide:
	case esmHide_SlippingAutoShow:
	{
		int nDistance = 0.0;
		m_enHideDirection = getShortestDirection(nDistance);
		if (m_enShowMode == esmNormal_EdgeAutoHide && nDistance - m_nAttachBorder > 0)
			break;

		switch (m_enHideDirection)
		{
		case ehdTop:
			while (this->pos().y() + this->height() > m_nAttachBorder)
			{
				int nH = this->height();
				int nY = this->pos().y() - m_nHideSpeed;
				if (nY + this->height() < m_nAttachBorder) nY = m_nAttachBorder - this->height();
				this->move(this->pos().x(), nY);
			}
			break;
		case ehdLeft:
			while (this->pos().x() + this->width() > m_nAttachBorder)
			{
				int nX = this->pos().x() - m_nHideSpeed;
				if (nX + this->width() < m_nAttachBorder) nX = m_nAttachBorder - this->width();
				this->move(nX, this->pos().y());
			}
			break;
		case ehdRight:
			while (this->pos().x() < m_oScreenRect.width() - m_nAttachBorder)
			{
				int nX = this->pos().x() + m_nHideSpeed;
				if (nX > m_oScreenRect.width() - m_nAttachBorder) nX = m_oScreenRect.width() - m_nAttachBorder;
				this->move(nX, this->pos().y());
			}
			break;
		case ehdBottom:
			while (this->pos().y() < m_oScreenRect.height() - m_nAttachBorder)
			{
				int nY = this->pos().y() + m_nHideSpeed;
				if (nY > m_oScreenRect.height() - m_nAttachBorder) nY = m_oScreenRect.height() - m_nAttachBorder;
				this->move(this->pos().x(), nY);
			}
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}

void VSFloatingWidget::showWidgetSliding()
{
	switch (m_enHideDirection)
	{
	case ehdTop:
		while (this->pos().y() < -m_nAttachBorder)
		{
			int nY = this->pos().y() + m_nHideSpeed;
			if (nY > -m_nAttachBorder) nY = -m_nAttachBorder;
			this->move(this->pos().x(), nY);
		}
		break;
	case ehdLeft:
		while (this->pos().x() < -m_nAttachBorder)
		{
			int nX = this->pos().x() + m_nHideSpeed;
			if (nX > -m_nAttachBorder) nX = -m_nAttachBorder;
			this->move(nX, this->pos().y());
		}
		break;
	case ehdRight:
		while (this->pos().x() + this->width() > m_oScreenRect.width() + m_nAttachBorder)
		{
			int nX = this->pos().x() - m_nHideSpeed;
			if (nX + this->width() < m_oScreenRect.width() + m_nAttachBorder) nX = m_oScreenRect.width() + m_nAttachBorder - this->width();
			this->move(nX, this->pos().y());
		}
		break;
	case ehdBottom:
		while (this->pos().y() + this->height() > m_oScreenRect.height() + m_nAttachBorder)
		{
			int nY = this->pos().y() - m_nHideSpeed;
			if (nY + this->height() < m_oScreenRect.height() + m_nAttachBorder) nY = m_oScreenRect.height() + m_nAttachBorder - this->height();
			this->move(this->pos().x(), nY);
		}
		break;
	}
}

void VSFloatingWidget::initWidgetProperty()
{
	// 浮窗不能让程序退出
	this->setAttribute(Qt::WA_QuitOnClose, true);

	// 窗口背景透明度（启用Alpha通道）
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	// 无任务栏图标
	this->setWindowFlags(this->windowFlags() | Qt::Tool );

	// 无边框
	this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
}

void VSFloatingWidget::setIgnoreAllEvent(bool bIgnore /*= true*/)
{
	if (this->isHidden())
		return;

	if (bIgnore)
	{
		SetWindowLong((HWND)this->winId()
			, GWL_EXSTYLE
			, GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT/*忽略一切消息（WM_PAINT除外）*/);
	}
	else
	{
		SetWindowLong((HWND)this->winId()
			, GWL_EXSTYLE 
			, GetWindowLong((HWND)winId(), GWL_EXSTYLE) & ~WS_EX_TRANSPARENT/*忽略一切消息（WM_PAINT除外）*/);
	}
}

bool VSFloatingWidget::event(QEvent* event)
{
	switch (m_enShowMode)
	{
	case esmShowFixed_SlippingAutoHide:
		if (event->type() == QEvent::HoverMove || event->type() == QEvent::Enter)
		{
			hideWidgetSliding();
		}
		else if (event->type() == QEvent::Leave)
		{
			QTimer::singleShot(5000, this, &VSFloatingWidget::showWidgetSliding);
		}
		break;
	case esmHide_SlippingAutoShow:
	case esmNormal_EdgeAutoHide:
		if (event->type() == QEvent::HoverMove || event->type() == QEvent::Enter)
		{
			showWidgetSliding();
		}
		else if (event->type() == QEvent::Leave)
		{
			hideWidgetSliding();
		}
		break;
	case esmHide:
	case esmShowFixed:
	case esmNormal:	
	default:
		break;
	}

	return __super::event(event);
}
