#ifndef SRMCPUTEMPERATUREOBSERVER_H_5E8031EF_46E0_45DD_B472_C44951B51DAD
#define SRMCPUTEMPERATUREOBSERVER_H_5E8031EF_46E0_45DD_B472_C44951B51DAD
/************************************************************************/
/* CPU温度单元，从Service中读的                                                                     */
/************************************************************************/
#include "Observer/SRMServiceReadObserverBase.h"

class SRMCPUTemperatureObserver : public SRMServiceReadObserverBase
{
public:
	SRMCPUTemperatureObserver();
	~SRMCPUTemperatureObserver();
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
};

#endif //SRMCPUTEMPERATUREOBSERVER_H_5E8031EF_46E0_45DD_B472_C44951B51DAD