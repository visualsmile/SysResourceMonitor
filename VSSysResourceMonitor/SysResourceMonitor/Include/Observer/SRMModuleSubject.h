#ifndef SRMMODULESUBJECT_H_C34DA930_C79E_4710_B025_4E5B70F9C1F5
#define SRMMODULESUBJECT_H_C34DA930_C79E_4710_B025_4E5B70F9C1F5
/************************************************************************/
/* 负责管理观察者，并通知观察者更新数据                                                                    */
/************************************************************************/

#include <QObject>
#include <QList>
#include <QTimer>
#include <QLabel>

class SRMObserverBase;
class ISRMCustomSettingInf;
class VSSharedMemStruct;
class QMenu;

class SRMObserverFactory
{
public:
	virtual SRMObserverBase* createObject() = 0;
};

class SRMModuleSubject : public QObject
{
	Q_OBJECT

public:// 代理的一些事情
	//************************************
	// Remark:    通知更新
	// FullName:  SRMModuleSubject::doOntify
	// Returns:   void
	// Parameter: VSSharedMemStruct * pSharedMemStruct	标尺控件
	// Parameter: QList<QLabel * > & oLabelList			浮窗控件
	//************************************
	void doOntify(VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList);
	//************************************
	// Remark:    注册右键菜单
	// FullName:  SRMModuleSubject::doRegTrayMenu
	// Returns:   void
	// Parameter: QMenu * pTrayMenu
	//************************************
	void doRegTrayMenu(QMenu* pTrayMenu);
	//************************************
	// Remark:    统计选中的条目，用于标尺、浮窗控件初始化
	// FullName:  SRMModuleSubject::doCheckedCount
	// Returns:   int
	//************************************
	int doCheckedCount();
	//************************************
	// Remark:    获取自定义设置接口，用于自定义设置功能
	// FullName:  SRMModuleSubject::doCustomSettingInfList
	// Returns:   QList<ISRMCustomSettingInf*>
	//************************************
	QList<ISRMCustomSettingInf*> doCustomSettingInfList();	

public:// 资源管理
	static SRMModuleSubject* getInstance();
	void init();
	void release();

public:// 观察者注册用
	void regObserverFactory(SRMObserverFactory* pObserverFactory);

private:
	SRMModuleSubject();
	~SRMModuleSubject();	

private:
	QList<SRMObserverBase*> m_oObserverList;
	QList<SRMObserverFactory*> m_oObserverFactoryList; // 增加一个factory，避免 observer被过早初始化
};

#define REG_SRM_OBSERVER(observer)\
class observer##Factory : public SRMObserverFactory\
{\
public:\
	SRMObserverBase* createObject() override\
	{\
		return new observer;\
	}\
};\
class observer##Register\
{\
public:\
	observer##Register()\
	{\
		SRMModuleSubject::getInstance()->regObserverFactory(new observer##Factory);\
	};\
};\
static observer##Register s_observer##Register;

#endif //SRMMODULESUBJECT_H_C34DA930_C79E_4710_B025_4E5B70F9C1F5
