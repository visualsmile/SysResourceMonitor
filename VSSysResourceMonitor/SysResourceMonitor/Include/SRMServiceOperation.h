#ifndef SRMSERVICEOPERATION_H_6BA47514_3A40_4759_9F1A_0A6C2309D63F
#define SRMSERVICEOPERATION_H_6BA47514_3A40_4759_9F1A_0A6C2309D63F
/************************************************************************/
/* 用于服务管理                                                                     */
/************************************************************************/
#include <QString>
#include <windows.h>

class SRMServiceOperation
{
public:
	static SRMServiceOperation* getInstance();
	~SRMServiceOperation();
	bool serviceIsRunning();
	bool unregService();
	bool regService();
	bool startService();
	bool stopService();

private:
	bool chekService();
	bool execOp(int nIndex, QString sOperationName);
	HANDLE openServiceMutex(int nRetryTimes);

private:
	bool m_bServiceIsRunning;

private:
	SRMServiceOperation();
};

#endif //SRMSERVICEOPERATION_H_6BA47514_3A40_4759_9F1A_0A6C2309D63F