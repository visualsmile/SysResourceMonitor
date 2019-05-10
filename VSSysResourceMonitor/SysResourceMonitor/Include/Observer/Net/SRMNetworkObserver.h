#ifndef SRMNETWORKOBSERVER_H_602C161A_DF67_4F1A_9E82_A5E2EE71D319
#define SRMNETWORKOBSERVER_H_602C161A_DF67_4F1A_9E82_A5E2EE71D319
/************************************************************************/
/* 网速读取模块     
依赖了 Iphlpapi.lib 模块
*/
/************************************************************************/

#include "Observer/SRMObserverBase.h"
#include <QSet>
#include <windows.h>
#include <iphlpapi.h>

class SRMNetworkObserver : public SRMObserverBase
{
public:
	SRMNetworkObserver();
	~SRMNetworkObserver();

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
	bool initAdapterTable();
	QSet<std::wstring> getPCI();

private:
	void readNetValue();

private:
	double m_dCurDownloadSpeed;
	double m_dCurUploadSpeed;
	PMIB_IFTABLE m_pAdapterTable;
	QSet<DWORD> m_oAdapterIDSet;
};

#endif //SRMNETWORKOBSERVER_H_602C161A_DF67_4F1A_9E82_A5E2EE71D319