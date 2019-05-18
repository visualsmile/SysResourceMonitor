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

// 自定义设置接口
class ISRMCustomSettingInf
{
	friend class SRMCustomSettingItem;
private:// 设置
	virtual int v1() = 0;
	virtual int v2() = 0;
	virtual void setV1(int nV) = 0;
	virtual void setV2(int nV) = 0;
	virtual QColor color1() = 0;
	virtual QColor color2() = 0;
	virtual QColor color3() = 0;
	virtual void setColor1(QColor& oColor) = 0;
	virtual void setColor2(QColor& oColor) = 0;
	virtual void setColor3(QColor& oColor) = 0;
private:// 提示信息
	virtual QString customSettingDescription() = 0;
};

// 观察者基类
class SRMObserverBase : public QObject, public ISRMCustomSettingInf
{
	Q_OBJECT

public:
	SRMObserverBase(QString sObserverClassName);
	virtual ~SRMObserverBase();

public:// 必须实现的接口
	//************************************
	// Remark:    返回初始化结果 初始化失败的，不会生效
	// FullName:  SRMObserverBase::init
	// Returns:   bool
	//************************************
	virtual bool init() = 0;
	//************************************
	// Remark:    通知更新
	// FullName:  SRMObserverBase::update
	// Returns:   void
	// Parameter: int nIndex								界面位置，下标
	// Parameter: VSSharedMemStruct * pSharedMemStruct		使用下标来更新自己的标尺数据
	// Parameter: QList<QLabel * > & oLabelList				使用下啊表来更新自己的浮窗信息
	//************************************
	virtual void update(int nIndex, VSSharedMemStruct *pSharedMemStruct, QList<QLabel*> &oLabelList) = 0;
	//************************************
	// Remark:    注册托盘区菜单
	// FullName:  SRMObserverBase::regTrayMenu
	// Returns:   void
	// Parameter: QMenu * pTrayMenu							父菜单
	//************************************
	virtual void regTrayMenu(QMenu *pTrayMenu) = 0;
	//************************************
	// Remark:    排序码，决定了控件顺序
	// FullName:  SRMObserverBase::order
	// Returns:   int
	//************************************
	virtual int order() = 0;

public:// 包含子的观察者，需要实现
	//************************************
	// Remark:    返回自定义设置接口，如果有子，可以连子一块返回
	// FullName:  SRMObserverBase::customSettingInf
	// Returns:   QList<ISRMCustomSettingInf*>
	//************************************
	virtual QList<ISRMCustomSettingInf*> customSettingInfList();
	//************************************
	// Remark:    选中的条目数量，如果有子，需要考虑子的选中数量，否则返回1
	//			  外面会根据选择的条目，构造任务栏标尺、浮窗中的控件
	// FullName:  SRMObserverBase::getCheckedCount
	// Returns:   int
	//************************************
	virtual int checkedCount() const;

protected:// 供子类调用的快捷函数
	void regTrayMenu(QMenu* pTrayMenu, QString sTrayActionName);	
	QColor fillCollorByValue(int nValue, VSShareMemTextNode* pValueNode);	

private:// 自定义设置相关
	virtual int v1() override;
	virtual int v2() override;
	virtual void setV1(int nV) override;
	virtual void setV2(int nV) override;
	virtual QColor color1() override;
	virtual QColor color2() override;
	virtual QColor color3() override;
	virtual void setColor1(QColor& oColor) override;
	virtual void setColor2(QColor& oColor) override;
	virtual void setColor3(QColor& oColor) override;

private:
	void readConfig();
	void actionStateChanged(); // slot
	void setIsChecked(bool bChecked);

private:
	int m_nV1;
	int m_nV2;
	QColor m_oColor1;
	QColor m_oColor2;
	QColor m_oColor3;
	bool m_bIsChecked;
	QString m_sObserverClassName;
};

#endif //SRMOBSERVERBASE_H_161F24E2_6327_472C_AEA5_A8CA6EED6519