#ifndef SRMDISKRATESUBOBSERVER_H_C37EDFB1_B306_4E9E_AF67_C54882276BC0
#define SRMDISKRATESUBOBSERVER_H_C37EDFB1_B306_4E9E_AF67_C54882276BC0
/************************************************************************/
/* 各个磁盘IO使用率，本身不注册，由SRMDiskRateObserver 管理
	SubObserver 主要用于刷新显示、保存配置等，不读数据
*/
/************************************************************************/
#include "Observer\SRMObserverBase.h"

class SRMDiskRateSubObserver : public SRMObserverBase
{
public:
	SRMDiskRateSubObserver(QString sPartitionName, QString sTrayMenuName);
	~SRMDiskRateSubObserver();

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

public:
	void updateInfo(QString sPartitionName, int nRate);

private:
	QString m_sPartitionName;
	QString m_sTrayMenuName;
	int m_nRate;
};

#endif //SRMDISKRATESUBOBSERVER_H_C37EDFB1_B306_4E9E_AF67_C54882276BC0