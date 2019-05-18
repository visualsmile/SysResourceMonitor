#ifndef SRMGPUTEMPERATUREOBSERVER_H_136B2060_1A58_4DF2_97BB_3CCDFA4DE3E2
#define SRMGPUTEMPERATUREOBSERVER_H_136B2060_1A58_4DF2_97BB_3CCDFA4DE3E2
/************************************************************************/
/* GPU温度                                                                   */
/************************************************************************/
#include "Observer/SRMObserverBase.h"

class SRMGPUInfoInf;

class SRMGPUTemperatureObserver : public SRMObserverBase
{
public:
	SRMGPUTemperatureObserver();
	~SRMGPUTemperatureObserver();

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

#endif //SRMGPUTEMPERATUREOBSERVER_H_136B2060_1A58_4DF2_97BB_3CCDFA4DE3E2