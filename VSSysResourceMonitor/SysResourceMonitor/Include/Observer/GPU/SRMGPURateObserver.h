#ifndef SRMGPURATEOBSERVER_H_28F5CF90_5408_47D0_B748_FFE7CBAE8E0E
#define SRMGPURATEOBSERVER_H_28F5CF90_5408_47D0_B748_FFE7CBAE8E0E
/************************************************************************/
/* 显卡速率                                                                     */
/************************************************************************/
#include "Observer/SRMObserverBase.h"

enum EnGPUType
{
	egtNune = 0,
	egtNvidia,
	egtAMD
};

class SRMGPURateObserver : public SRMObserverBase
{
public:
	SRMGPURateObserver();
	~SRMGPURateObserver();

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
	QString customSettingDescription() override;

private:
	int getGPURate();

private:
	EnGPUType m_enGPUType;
};

#endif //SRMGPURATEOBSERVER_H_28F5CF90_5408_47D0_B748_FFE7CBAE8E0E
