#ifndef SRMSERVICEREADOBSERVERBASE_H_850BDCDF_78E4_402A_8C35_B4442BB7CBD1
#define SRMSERVICEREADOBSERVERBASE_H_850BDCDF_78E4_402A_8C35_B4442BB7CBD1

/************************************************************************/
/* 依赖Service的观察者基类
需要从Service中获取数据*/
/************************************************************************/

#include "SRMObserverBase.h"
#include <windows.h>

class SRMServiceReadObserverBase : public SRMObserverBase
{
public:
	SRMServiceReadObserverBase(QString sReadMutexSemaphoreName, QString sShareMemoryName, QString sObserverClassName);
	virtual ~SRMServiceReadObserverBase();
	bool init();

protected:
	int readServiceData();

private: // 共享内存相关
	HANDLE m_hFile;							// 共享区句柄
	HANDLE m_hReadMutexSemaphore;			// 全局互斥量
	void* m_pBuffer;						// 共享区指针
	QString m_sReadMutexSemaphoreName;
	QString m_sShareMemoryName;
};

#endif //SRMSERVICEREADOBSERVERBASE_H_850BDCDF_78E4_402A_8C35_B4442BB7CBD1
