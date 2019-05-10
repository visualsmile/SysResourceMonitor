#ifndef SRMSERVICEOBSERVERBASE_H_94631F19_4A82_41A8_8305_8F659B242279
#define SRMSERVICEOBSERVERBASE_H_94631F19_4A82_41A8_8305_8F659B242279
/************************************************************************/
/* 观察者基类                                                                     */
/************************************************************************/
#include <QObject>
#include <windows.h>

class SRMServiceObserverBase : public QObject
{
	Q_OBJECT
public:
	SRMServiceObserverBase(QString sWriteMutexSemaphoreName, QString sShareMemoryName);
	virtual ~SRMServiceObserverBase();

public:
	// 返回初始化结果 初始化失败的，直接删除
	virtual bool init();
	// 通知更新
	virtual void update() = 0;

protected:
	// 只能传一个int值
	void writeValue(int nValue);

private:
	void initSharedMemory();
	void freeSharedMemory();
	void* m_pSharedBuffer;
	HANDLE m_hMapFile;
	HANDLE m_hWriteSemaphore;
	QString m_sMutexSemaphoreName;
	QString m_sShareMemoryName;
};

#endif //SRMSERVICEOBSERVERBASE_H_94631F19_4A82_41A8_8305_8F659B242279