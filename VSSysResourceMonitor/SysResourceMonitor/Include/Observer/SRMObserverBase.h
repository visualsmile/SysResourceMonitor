#ifndef SRMOBSERVERBASE_H_161F24E2_6327_472C_AEA5_A8CA6EED6519
#define SRMOBSERVERBASE_H_161F24E2_6327_472C_AEA5_A8CA6EED6519
/************************************************************************/
/* 普通观察者基类，派生类直接可以完成相应数据获取
区别于 SRMServiceReadObserverBase				*/
/************************************************************************/
#include <QString>
#include <QColor>
#include <QObject>
#include <QLabel>
#include "VSDeskBandOperation.h"

class QMenu;

class SRMObserverBase : public QObject
{
	Q_OBJECT

public:
	SRMObserverBase(QString sObserverClassName);
	virtual ~SRMObserverBase();

public:
	// 返回初始化结果 初始化失败的，直接删除
	virtual bool init() = 0;
	// 通知更新
	virtual void update(int nIndex, VSSharedMemStruct *pSharedMemStruct, QList<QLabel*> &oLabelList) = 0;
	// 注册托盘区菜单
	virtual void regTrayMenu(QMenu *pTrayMenu) = 0;
	// 排序码
	virtual int order() = 0;
	// 自定义设置界面的提示信息
	virtual QString customSettingDescription() = 0;

protected:
	virtual void actionStateChanged();
	virtual void regTrayMenu(QMenu *pTrayMenu, QString sTrayActionName);

public:// 设置相关
	int v1();
	int v2();
	QColor color1();
	QColor color2();
	QColor color3();
	bool isChecked();
	void setV1(int nV);
	void setV2(int nV);
	void setColor1(QColor& oColor);
	void setColor2(QColor& oColor);
	void setColor3(QColor& oColor);
	void setIsChecked(bool bChecked);

protected:
	QColor fillCollorByValue(int nValue, VSShareMemTextNode* pValueNode);

private:
	void readConfig();

private:
	int m_nV1;
	int m_nV2;
	QColor m_oColor1;
	QColor m_oColor2;
	QColor m_oColor3;
	bool m_bIsChecked;

protected:
	QString m_sObserverClassName;
};

#endif //SRMOBSERVERBASE_H_161F24E2_6327_472C_AEA5_A8CA6EED6519