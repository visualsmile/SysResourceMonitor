#ifndef SRMCUSTOMSETTINGWIDGET_H_E85D8CEB_7F23_46EF_BF4A_5A8BE756D11C
#define SRMCUSTOMSETTINGWIDGET_H_E85D8CEB_7F23_46EF_BF4A_5A8BE756D11C
/************************************************************************/
/* 自定义设置窗口                                                                     */
/************************************************************************/
#include <QFrame>
#include <QDialog>
#include <QColorDialog>
#include <QColor>
#include <QEvent>

class ISRMCustomSettingInf;

// QColorDialog 关闭时，会关闭程序，处理下
class SRMColorDialog : public QColorDialog
{
public:
	SRMColorDialog(QColor& oColor, QWidget* pParent = nullptr);
	~SRMColorDialog();
};

class SRMCustomSettingItem : public QFrame
{
public:
	SRMCustomSettingItem(ISRMCustomSettingInf* pCustomSettingInf, QWidget* pParent);
	~SRMCustomSettingItem();

private:
	void initLayout();
	void initProp();

private:
	void colorPushbuttonClickedSlot();
	void v1ValueChangedSlot(QString sValue);
	void v2ValueChangedSlot(QString sValue);

private:
	ISRMCustomSettingInf* m_pCustomSettingInf;
};

class SRMCustomSettingDialog : public QDialog
{
public:
	SRMCustomSettingDialog(QWidget* pParent = nullptr);
	~SRMCustomSettingDialog();

private:
	void initProp();
	void initLayout();
};

#endif //SRMCUSTOMSETTINGWIDGET_H_E85D8CEB_7F23_46EF_BF4A_5A8BE756D11C