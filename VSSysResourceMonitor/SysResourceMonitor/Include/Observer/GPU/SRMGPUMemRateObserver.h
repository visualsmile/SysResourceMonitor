#ifndef SRMGPUMEMRATEOBSERVER_H_3CFC26CB_F532_4FCE_9647_BD495AAF1EDD
#define SRMGPUMEMRATEOBSERVER_H_3CFC26CB_F532_4FCE_9647_BD495AAF1EDD
/************************************************************************/
/* 显存使用率                                                                     */
/************************************************************************/
#include "Observer/SRMObserverBase.h"

class SRMGPUInfoInf;

class SRMGPUMemRateObserver : public SRMObserverBase
{
public:
	SRMGPUMemRateObserver();
	~SRMGPUMemRateObserver();

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
	SRMGPUInfoInf* m_pGPUInfoInf;
};

#endif //SRMGPUMEMRATEOBSERVER_H_3CFC26CB_F532_4FCE_9647_BD495AAF1EDD