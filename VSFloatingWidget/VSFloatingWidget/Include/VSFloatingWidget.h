#ifndef VSFLOATINGWIDGET_H_2BAB0890_ED39_4BF8_98A9_1AE6AADBEE38
#define VSFLOATINGWIDGET_H_2BAB0890_ED39_4BF8_98A9_1AE6AADBEE38
/************************************************************************/
/* 定义了一种浮窗的实现                                                                     */
/************************************************************************/
#include <QWidget>
#include <QPoint>
#include <QTimer>
#include <QColor>

// 窗口展示模式
enum EnVSFWShowMode
{
	esmHide,						// 不显示
	esmNormal,						// 普通窗口模式，显示，响应鼠标拖动（普通窗口）
	esmNormal_EdgeAutoHide,			// 贴边自动隐藏，响应鼠标拖动，贴边时会自动隐藏（类似QQ贴边隐藏）
	esmShowFixed,					// 固定不可选中，显示，不响应鼠标事件，不能移动（固定浮窗，窗口鼠标可穿透）
	esmShowFixed_SlippingAutoHide,	// 触碰自动隐藏，捕捉到鼠标后隐藏，不能移动
	esmHide_SlippingAutoShow,		// 始终自动隐藏，贴边隐藏，捕捉到鼠标后显示，不能移动
};

// 窗口依附位置枚举
enum EnVSFWPositionType
{
	eptTopMid = 0,
	eptTopLeft,
	eptTopRight,
	eptBottomMid,
	eptBottomLeft,
	eptBottomRight,
};

// 自动移动方向枚举（临时）
enum EnVSFWHideDirection
{
	ehdTop = 0,
	ehdLeft,
	ehdRight,
	ehdBottom,
};

class VSFloatingWidget : public QWidget
{
	Q_OBJECT

public:
	//************************************
	// Remark:	  EnVSFWShowMode 指定了窗口工作模式
	// FullName:  VSFloatingWidget::VSFloatingWidget
	// Parameter: EnVSFWShowMode enShowMode
	// Parameter: EnAttachType enAttachType
	// Parameter: QPoint & oCurPosition
	// Parameter: QWidget * parent
	//************************************
	VSFloatingWidget(EnVSFWShowMode enShowMode = esmNormal, QPoint &oCurPosition = QPoint(0, 0), QWidget *parent = nullptr);
	virtual ~VSFloatingWidget();

public:
public:
	//************************************
	// Remark:    覆盖了基类的show，不要用QWidet的指针show窗口
	// FullName:  VSFloatingWidget::show
	// Returns:   void
	//************************************
	void show();

	//************************************
	// Remark:    窗口展示模式
	// FullName:  VSFloatingWidget::setShowMode
	// Returns:   void
	// Parameter: EnVSFWShowMode enShowMode
	//************************************
	virtual void setShowMode(EnVSFWShowMode enShowMode);
	EnVSFWShowMode getShowMode();

	//************************************
	// Remark:    计算窗口位置
	// FullName:  VSFloatingWidget::calcPosition
	// Returns:   QT_NAMESPACE::QPoint
	// Parameter: EnVSFWPositionType enAttachType
	//************************************
	QPoint calcPosition(EnVSFWPositionType enAttachType);
	// 暂时停止置顶
	void pauseTopOn();
	// 恢复置顶
	void resumeTopOn();

public:
	//************************************
	// Remark:    隐藏、显示窗口时的动画速度。以每次移动的像素数量为准
	// FullName:  VSFloatingWidget::setSlidingSpeed
	// Returns:   void
	// Parameter: int nHideSpeed
	//************************************
	void setSlidingSpeed(int nHideSpeed = 2);
	//************************************
	// Remark:    边缘线。窗口移出桌面后，会留下这个值的像素点影像，在这个值之内，窗口会自动吸附到边缘
	// FullName:  VSFloatingWidget::setAttachBoder
	// Access:    public 
	// Returns:   void
	// Parameter: int nAttachBorder
	//************************************
	void setAttachBoder(int nAttachBorder = 2);
	//************************************
	// Remark:    设置窗口整体不透明度
	// FullName:  VSFloatingWidget::setWindowsOpacity
	// Returns:   void
	// Parameter: double dOpacity			【0-1】，为0时完全透明
	//************************************
	void setWidgetOpacity(double dOpacity = 1);
	//************* 设置窗口背景色，支持alpha通道***********************
	// Remark:    
	// FullName:  VSFloatingWidget::setBackgroundColor
	// Returns:   void
	// Parameter: QColor & oColor			默认黑色
	// Parameter: double dAlpha				【0-1】，为0时完全透明
	//************************************
	void setBackgroundColor(QColor &oColor = QColor(0, 0, 0), double dAlpha = 0);
	//************************************
	// Remark:    设置置顶频率，默认每秒60次
	// FullName:  VSFloatingWidget::setFrequency
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int nFrequence
	//************************************
	void setFrequency(int nFrequence = 60);

private:
	virtual bool event(QEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) override;

protected:
	void stackWidgetOnTop();
	void hideWidgetSliding();
	void showWidgetSliding();

private:
	void initWidgetProperty();
	void setIgnoreAllEvent(bool bIgnore = true);

private:// 状态定义
	EnVSFWShowMode m_enShowMode;

private:// 临时
	QPoint m_oOldPos;							// 移动窗口时，鼠标位置记录
	QRect m_oScreenRect;
	EnVSFWHideDirection m_enHideDirection;
	int m_nPausedCount;							// 记录暂停置顶次数

private:// 不变的
	int m_nHideSpeed;							// 隐藏、显示窗口时的动画速度。以每次移动的像素数量为准
	int m_nAttachBorder;						// 边缘线，窗口移出后，会留下这个值的像素点影像，在这个值之内，窗口会自动吸附到边缘
	double m_dWidgetOpacity;					// 窗口不透明度
	QColor m_oBackgroundColor;					// 窗口背景颜色
	QTimer m_oTimer;							// 计时器，用于定时置顶窗口
};

#endif //VSFLOATINGWIDGET_H_2BAB0890_ED39_4BF8_98A9_1AE6AADBEE38