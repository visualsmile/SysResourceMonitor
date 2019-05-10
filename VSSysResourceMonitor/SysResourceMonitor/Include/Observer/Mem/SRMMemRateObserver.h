#ifndef SRMMEMRATEOBSERVER_H_BD5E3A7E_9EC3_4F22_B78D_5CB8A0A21C0B
#define SRMMEMRATEOBSERVER_H_BD5E3A7E_9EC3_4F22_B78D_5CB8A0A21C0B
/************************************************************************/
/* 内存占用                                                                     */
/************************************************************************/

#include "Observer/SRMObserverBase.h"

class SRMMemRateObserver : public SRMObserverBase
{
public:
	SRMMemRateObserver();
	~SRMMemRateObserver();

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
	int getMemRate();
};

#endif //SRMMEMRATEOBSERVER_H_BD5E3A7E_9EC3_4F22_B78D_5CB8A0A21C0B