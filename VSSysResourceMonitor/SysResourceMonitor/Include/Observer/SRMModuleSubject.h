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

public:
	static SRMModuleSubject* getInstance();
	void init();
	int getCheckedCount();
	void doOntify(VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList);
	void regTrayMenu(QMenu* pTrayMenu);
	QList<SRMObserverBase*>& getObserverList();

public:
	void regObserverFactory(SRMObserverFactory* pObserverFactory);

private:
	SRMModuleSubject();
	~SRMModuleSubject();	

private:
	QList<SRMObserverBase*> m_oObserverList;
	QList<SRMObserverFactory*> m_oObserverFactoryList; // 增加一个factory吧，避免 observer被过早初始化
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
