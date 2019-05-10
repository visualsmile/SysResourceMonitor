#ifndef SRMCPURATEOBSERVER_H_CBB28C7F_C36A_43F5_A6EC_C30C4FADEBA2
#define SRMCPURATEOBSERVER_H_CBB28C7F_C36A_43F5_A6EC_C30C4FADEBA2
/************************************************************************/
/* CPU使用率计算单元                                                                     */
/************************************************************************/
#include "Observer/SRMObserverBase.h"
#include <windows.h>

class SRMCPURateObserver : public SRMObserverBase
{
public:
	SRMCPURateObserver();
	~SRMCPURateObserver();

public:
	// 返回初始化结果 初始化失败的，直接删除
	virtual bool init() override;
	// 通知更新
	virtual void update(int nIndex, VSSharedMemStruct* pSharedMemStruct, QList<QLabel*>& oLabelList) override;
	// 注册托盘区菜单
	virtual void regTrayMenu(QMenu* pTrayMenu) override;
	// 排序码
	virtual int order() override;
	// 自定义设置界面的提示信息
	virtual QString customSettingDescription() override;

private:
	int getCPURate();

private:
	FILETIME m_oPreIdleTime;
	FILETIME m_oPreKernelTime;
	FILETIME m_oPreUserTime;
};

#endif //SRMCPURATEOBSERVER_H_CBB28C7F_C36A_43F5_A6EC_C30C4FADEBA2