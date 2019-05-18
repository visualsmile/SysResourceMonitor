#ifndef SRMDISKRATEOBSERVER_H_3E936524_6BD6_45BA_9453_64D317DBB998
#define SRMDISKRATEOBSERVER_H_3E936524_6BD6_45BA_9453_64D317DBB998
/************************************************************************/
/* 磁盘io使用率
   使用 WMI 获取数据*/
/************************************************************************/
#include "Observer\SRMObserverBase.h"
#include <QString>
#include <QHash>
#include "Observer\Disk\SRMDiskRateSubObserver.h"

class IWbemRefresher;
class IWbemHiPerfEnum;

struct PartitionInfo
{
	QString sTrayMenuName;
	QString sPartitionName;
	int nUsingRate;
};

class SRMDiskRateObserver : public SRMObserverBase
{
public:
	SRMDiskRateObserver();
	~SRMDiskRateObserver();

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
	// 选中的条目数量
	virtual int checkedCount() const override;
	// 自定义设置
	virtual QList<ISRMCustomSettingInf*> customSettingInfList() override;

private:
	void updatePartitionInfo();
	void addAvgAndBusiest();
	SRMDiskRateSubObserver* findSubObserver(const PartitionInfo& oPartitionInfo);

private:
	IWbemRefresher* m_pWbemRefresher;
	IWbemHiPerfEnum* m_pWbemHiPerfEnum;

private:
	QList<PartitionInfo> m_oPartitionInfoList;
	QHash<QString, SRMDiskRateSubObserver*> m_oSubObserverHash;
};

#endif //SRMDISKRATEOBSERVER_H_3E936524_6BD6_45BA_9453_64D317DBB998
