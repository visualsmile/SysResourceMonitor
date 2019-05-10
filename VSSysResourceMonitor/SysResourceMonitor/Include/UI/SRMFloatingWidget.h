#ifndef SRMFLOATINGWIDGET_H_915F131C_9C76_4318_82DB_3936A41A7D5F
#define SRMFLOATINGWIDGET_H_915F131C_9C76_4318_82DB_3936A41A7D5F
/************************************************************************/
/* ¸¡´°                                                                     */
/************************************************************************/
#include "VSFloatingWidget.h"
#include "SRMConfig.h"
#include <QList>

class QLabel;

class SRMFloatingWidget : public VSFloatingWidget
{
public:
	SRMFloatingWidget(QWidget* pParent = nullptr);
	~SRMFloatingWidget();
	QList<QLabel*> updateLayout();
	void changePositionType(EnVSFWPositionType enType);

public:
	virtual void setShowMode(EnVSFWShowMode enShowMode) override;

private:
	void saveConfig();

private:
	QList<QLabel*> m_oLabelList;
};

#endif //SRMFLOATINGWIDGET_H_915F131C_9C76_4318_82DB_3936A41A7D5F